module FPlusApiSearch exposing (..)

-- where -- todo remove this comment

import Database exposing (Function, functions)
import TypeSignature
import Debug
import Html exposing (..)
import Html.App exposing (program)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Random
import String
import StringDistance


main =
    program
        { init = initModelAndCommands
        , update = update
        , subscriptions = always Sub.none
        , view = view
        }


maxVisibleFunctions : Int
maxVisibleFunctions =
    20


initModelAndCommands : ( Model, Cmd Msg )
initModelAndCommands =
    ( defaultModel, cmdGetRandomNumbers )


normalizeFunctionSignature function =
    let
        maybeSig =
            function.signature
                |> TypeSignature.parseSignature
    in
        case maybeSig of
            Just sig ->
                { function
                    | signature =
                        sig
                            |> TypeSignature.normalizeSignature
                            |> TypeSignature.showSignature
                }

            Nothing ->
                Debug.log
                    ("Error parsing signature of "
                        ++ function.name
                        ++ ": "
                        ++ function.signature
                    )
                    function


functions : List Function
functions =
    List.map normalizeFunctionSignature Database.functions


functionCnt : Int
functionCnt =
    List.length functions


cmdGetRandomNumbers : Cmd Msg
cmdGetRandomNumbers =
    Random.list maxVisibleFunctions (Random.int 0 (functionCnt))
        |> Random.generate RandomNumbers


type alias Model =
    { query : String
    , searchResult : List ( Function, Float )
    }


defaultModel : Model
defaultModel =
    { query = ""
    , searchResult = []
    }


type Msg
    = NoOp
    | RandomNumbers (List Int)
    | UpdateQuery String


update : Msg -> Model -> ( Model, Cmd Msg )
update action model =
    case action of
        NoOp ->
            ( model, Cmd.none )

        UpdateQuery str ->
            if String.isEmpty str then
                ( { model
                    | query = str
                    , searchResult = []
                  }
                , cmdGetRandomNumbers
                )
            else
                ( { model
                    | query = str
                    , searchResult = searchFunctions str
                  }
                , Cmd.none
                )

        RandomNumbers nums ->
            ( { model
                | searchResult =
                    if String.isEmpty model.query then
                        getRandomSearchResult nums
                    else
                        model.searchResult
              }
            , Cmd.none
            )


nthElement : List a -> Int -> Maybe a
nthElement xs idx =
    List.drop idx xs |> List.head


getRandomSearchResult : List Int -> List ( Function, Float )
getRandomSearchResult chosenIdxs =
    chosenIdxs
        |> List.filterMap (nthElement functions)
        |> List.map (\x -> ( x, 0 ))


view : Model -> Html Msg
view model =
    div [ class "mainwrapper" ]
        [ div [ class "main" ]
            [ div [ class "githublink" ]
                [ let
                    url =
                        "https://github.com/dobiasd/"
                  in
                    a [ href url ]
                        [ div [ class "logo" ]
                            [ img [ class "logo", src "fplus.png" ] []
                            ]
                        , text url
                        ]
                ]
            , hr [] []
            , input
                [ placeholder "search query"
                , autofocus True
                , style [ ( "width", "500px" ) ]
                , onInput UpdateQuery
                ]
                []
            , hr [] []
            , model.searchResult |> showFunctions
            , hr [] []
            , showFooter
            ]
        ]


showFooter : Html Msg
showFooter =
    footer [ class "footer" ]
        [ text "Copyright © 2016 Tobias Hermann. All rights reserved."
        ]


cleanFunctionSignature : String -> String
cleanFunctionSignature str =
    let
        isSpace c =
            c == ' '
    in
        String.filter (isSpace >> not) str


searchFunctions : String -> List ( Function, Float )
searchFunctions query =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query f ))
    in
        ratedFunctions
            |> List.sortBy (\( _, rating ) -> 0 - rating)
            |> List.take maxVisibleFunctions
            |> List.filter (\( _, rating ) -> rating > 0)


boolToNum : Float -> Bool -> Float
boolToNum value b =
    if b then
        value
    else
        0


functionRating : String -> Function -> Float
functionRating query_orig function =
    let
        query =
            query_orig |> String.toLower

        queryWords =
            String.words query

        stringLengthFloat =
            String.length >> toFloat

        queryWordLengthSum =
            queryWords
                |> List.map stringLengthFloat
                |> List.sum

        wordRatingSum =
            queryWords
                |> List.map
                    (\queryWord ->
                        functionWordRating (stringLengthFloat queryWord / queryWordLengthSum)
                            function
                            queryWord
                    )
                |> List.sum

        -- todo: type rating with parsing
        --       cleanFunctionSignature destroys stuff like "Maybe a" anyway.
        typeRating =
            String.contains (cleanFunctionSignature query)
                (function.signature |> cleanFunctionSignature |> String.toLower)
                |> boolToNum 1000
    in
        wordRatingSum + typeRating


functionWordRating : Float -> Function -> String -> Float
functionWordRating weight function query =
    --StringDistance.sift3Distance query function.name
    let
        isSubStr =
            String.contains query function.name

        queryLength =
            String.length query |> toFloat

        functionNameLength =
            String.length function.name |> toFloat

        lengthDiff =
            queryLength - functionNameLength |> abs

        queryAndFunctionNameMaxLength =
            Basics.max queryLength functionNameLength

        relLengthDiff =
            lengthDiff / queryAndFunctionNameMaxLength

        nameRating =
            weight * Basics.max 0 (boolToNum 1000 isSubStr - 50 * relLengthDiff)

        docRating =
            String.contains query (String.toLower function.documentation) |> boolToNum 10
    in
        nameRating + docRating



--StringDistance.lcs (String.toList query) (String.toList function.name) |> List.length |> toFloat


showFunctions : List ( Function, Float ) -> Html Msg
showFunctions ratedFunctions =
    List.map showRatedFunction ratedFunctions
        |> List.intersperse (hr [] [])
        |> div [ class "functions" ]


stringToCode : String -> String -> Html Msg
stringToCode language str =
    let
        defOpts =
            Markdown.defaultOptions

        options =
            { defOpts | defaultHighlighting = Just language }

        taggedStr =
            "```\n" ++ str ++ "\n```"
    in
        Markdown.toHtmlWith options [] taggedStr


docFromString : String -> Html Msg
docFromString str =
    let
        taggedStr =
            "```" ++ str ++ "```"
    in
        Markdown.toHtmlWith Markdown.defaultOptions [] taggedStr


ratingToHtml : Float -> Html Msg
ratingToHtml rating =
    "search rating: " ++ toString (round rating) |> docFromString


showRatedFunction : ( Function, Float ) -> Html Msg
showRatedFunction ( function, rating ) =
    let
        functionNameAndSig =
            div [ class "functionnameandsig" ]
                [ function.name
                    ++ " : "
                    ++ function.signature
                    |> stringToCode "haskell"
                ]

        functionDocumentation =
            div [ class "functiondoc" ]
                [ function.documentation
                    |> docFromString
                ]

        functionDeclaration =
            div [ class "functiondecl" ]
                [ function.declaration
                    |> stringToCode "cpp"
                ]

        functionRating =
            div [ class "functionrating" ]
                [ rating
                    |> ratingToHtml
                ]
    in
        div [ class "function" ]
            [ functionNameAndSig
            , functionDocumentation
            , functionDeclaration
            , functionRating
            ]
