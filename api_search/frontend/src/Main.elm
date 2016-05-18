module FPlusApiSearch exposing (..)

import Database exposing (Function, functions)
import Html exposing (..)
import Html.App exposing (program)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import String
import StringDistance


main =
    program
        { init = initModelAndCommands
        , update = update
        , subscriptions = always Sub.none
        , view = view
        }

initModelAndCommands : ( Model, Cmd Msg )
initModelAndCommands =
    ( defaultModel, Cmd.none )


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
    | UpdateQuery String


update : Msg -> Model -> ( Model, Cmd Msg )
update action model =
    case action of
        NoOp ->
            (model, Cmd.none)

        UpdateQuery str ->
            ({ model
                | query = str
                , searchResult = searchFunctions str
            }, Cmd.none)


view : Model -> Html Msg
view model =
    div
        [ class "main" ]
        [ img [ src "fplus.png" ] []
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


showFooter : Html Msg
showFooter =
    footer
        [ class "footer" ]
        [ text "Copyright © 2016 Tobias Hermann. All rights reserved."
        ]


searchFunctions : String -> List ( Function, Float )
searchFunctions query =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query f ))
    in
        ratedFunctions
            |> List.sortBy (\( _, rating ) -> 0 - rating)
            |> List.take 20
            |> List.filter (\( _, rating ) -> rating > 0)


functionRating : String -> Function -> Float
functionRating query function =
    let
        queryWords = String.words query

        stringLengthFloat = String.length >> toFloat

        queryWordLengthSum =
            queryWords
                |> List.map stringLengthFloat
                |> List.sum
    in
        queryWords
            |> List.map
                (\queryWord ->
                    functionWordRating
                        (stringLengthFloat queryWord / queryWordLengthSum)
                        function
                        queryWord
                )
            |> List.sum


functionWordRating : Float -> Function -> String -> Float
functionWordRating weight function query =
    --StringDistance.sift3Distance query function.name
    let
        boolToNum value b =
            if b then
                value
            else
                0

        -- todo: rate type
        isSubStr = String.contains query function.name

        queryLength = String.length query |> toFloat

        functionNameLength = String.length function.name |> toFloat

        lengthDiff = queryLength - functionNameLength |> abs

        queryAndFunctionNameMaxLength = Basics.max queryLength functionNameLength

        relLengthDiff = lengthDiff / queryAndFunctionNameMaxLength

        nameRating = weight * Basics.max 0 (boolToNum 1000 isSubStr - 50 * relLengthDiff)

        docRating = String.contains query function.documentation |> boolToNum 10
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
        defOpts = Markdown.defaultOptions

        options = { defOpts | defaultHighlighting = Just language }

        taggedStr = "```\n" ++ str ++ "\n```"
    in
        Markdown.toHtmlWith options [] taggedStr


stringToDoc : String -> Html Msg
stringToDoc str =
    let
        taggedStr = "```" ++ str ++ "```"
    in
        Markdown.toHtmlWith Markdown.defaultOptions [] taggedStr


ratingToHtml : Float -> Html Msg
ratingToHtml rating =
    "search rating: " ++ toString rating |> stringToDoc


showRatedFunction : ( Function, Float ) -> Html Msg
showRatedFunction ( function, rating ) =
    let
        functionNameAndSig =
            div
                [ class "functionnameandsig" ]
                [ function.name
                    ++ " : "
                    ++ function.signature
                    |> stringToCode "elm"
                ]

        functionDocumentation =
            div
                [ class "functiondoc" ]
                [ function.documentation
                    |> stringToDoc
                ]

        functionDeclaration =
            div
                [ class "functiondecl" ]
                [ function.declaration
                    |> stringToCode "cpp"
                ]

        functionRating =
            div
                [ class "functionrating" ]
                [ rating
                    |> ratingToHtml
                ]
    in
        div
            [ class "function" ]
            [ functionNameAndSig
            , functionDocumentation
            , functionDeclaration
            , functionRating
            ]