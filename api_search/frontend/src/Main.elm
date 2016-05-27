module FPlusApiSearch exposing (..)

import Database exposing (Function, functions)
import TypeSignature
import Debug
import Html exposing (..)
import Html.App exposing (program)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Random
import Regex
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
    , querySig : Maybe TypeSignature.Signature
    , querySigStr : String
    , searchResult : List ( Function, Float )
    }


defaultModel : Model
defaultModel =
    { query = ""
    , querySig = Nothing
    , querySigStr = ""
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
                    , querySig = Nothing
                    , querySigStr = ""
                    , searchResult = []
                  }
                , cmdGetRandomNumbers
                )
            else
                let
                    newQuerySig =
                        str
                            |> cleanFunctionSignature
                            |> TypeSignature.parseSignature
                            |> Maybe.map TypeSignature.normalizeSignature

                    newQuerySigStr =
                        newQuerySig |> showMaybeSig
                in
                    ( { model
                        | query = str
                        , querySig = newQuerySig
                        , querySigStr = newQuerySigStr
                        , searchResult = searchFunctions str newQuerySigStr
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


showMaybeSig : Maybe TypeSignature.Signature -> String
showMaybeSig maybeSig =
    case maybeSig of
        Maybe.Just s ->
            TypeSignature.showSignature s

        Maybe.Nothing ->
            ""


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
                , autocomplete True
                , style [ ( "width", "500px" ) ]
                , onInput UpdateQuery
                ]
                []
            , if String.isEmpty model.querySigStr then
                div [ class "queryhelper" ]
                    [ text "You can search by function name, documentation tags or type signature" ]
              else
                div [ class "parsedsignature" ]
                    [ "as parsed type: " ++ model.querySigStr |> stringToCode "Haskell" ]
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


replaceInString : String -> String -> String -> String
replaceInString pattern replacement =
    Regex.replace Regex.All (Regex.regex pattern) (always replacement)


replaceSubMatchInString : String -> (String -> String) -> String -> String
replaceSubMatchInString pattern replacementFunc =
    let
        f { match, submatches } =
            case submatches of
                [ Maybe.Just sm ] ->
                    replacementFunc sm

                _ ->
                    match
    in
        Regex.replace Regex.All (Regex.regex pattern) f


replaceTwoSubMatchInString :
    String
    -> (String -> String -> String)
    -> String
    -> String
replaceTwoSubMatchInString pattern replacementFunc =
    let
        f { match, submatches } =
            case submatches of
                [ Maybe.Just sm1, Maybe.Just sm2 ] ->
                    replacementFunc sm1 sm2

                _ ->
                    match
    in
        Regex.replace Regex.All (Regex.regex pattern) f


applyUntilIdempotent : (String -> String) -> String -> String
applyUntilIdempotent f str =
    let
        res =
            f str
    in
        if res == str then
            res
        else
            applyUntilIdempotent f res


cleanFunctionSignature : String -> String
cleanFunctionSignature =
    let
        recursiveCases =
            replaceSubMatchInString "vector<([^<>]*)>" (\sm -> "[" ++ sm ++ "]")
                >> replaceSubMatchInString "list<([^<>]*)>" (\sm -> "[" ++ sm ++ "]")
                >> replaceSubMatchInString "set<([^<>]*)>" (\sm -> "Set " ++ sm)
                >> replaceTwoSubMatchInString "map<([^<>]*),([^<>]*)>" (\sm1 sm2 -> "Map " ++ sm1 ++ " " ++ sm2)
                >> replaceTwoSubMatchInString "pair<([^<>]*),([^<>]*)>" (\sm1 sm2 -> "(" ++ sm1 ++ ", " ++ sm2 ++ ")")
                |> applyUntilIdempotent
    in
        String.toLower
            >> replaceInString "std::" ""
            >> replaceInString "fplus::" ""
            >> recursiveCases
            >> replaceInString "unsigned int" "Int"
            >> replaceInString "unsigned" "Int"
            >> replaceInString "size_t" "Int"
            >> replaceInString "short" "Int"
            >> replaceInString "signed" "Int"
            >> replaceInString "long" "Int"
            >> replaceInString "integer" "Int"
            >> replaceInString "int" "Int"
            >> replaceInString "double" "Float"
            >> replaceInString "float" "Float"
            >> replaceInString "boolean" "Bool"
            >> replaceInString "char" "Char"
            >> replaceInString "bool" "Bool"
            >> replaceInString "maybe" "Maybe"
            >> replaceInString "either" "Result"
            >> replaceInString "result" "Result"
            >> replaceInString "map" "Map"
            >> replaceInString "dict" "Map"
            >> replaceInString "set" "Int"
            >> replaceInString "string" "String"
            >> replaceInString "String" "[Char]"


searchFunctions : String -> String -> List ( Function, Float )
searchFunctions query querySigStr =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query querySigStr f ))
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


functionRating : String -> String -> Function -> Float
functionRating queryOrig querySigStr function =
    let
        query =
            queryOrig |> String.toLower

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

        typeWeight =
            stringLengthFloat querySigStr / stringLengthFloat function.signature

        typeRating =
            String.contains querySigStr function.signature
                |> boolToNum 1000
                |> (*) typeWeight
    in
        wordRatingSum + typeRating


functionWordRating : Float -> Function -> String -> Float
functionWordRating weight function query =
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
