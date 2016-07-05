module FPlusApiSearch exposing (..)

import Database
import TypeSignature
import Html exposing (..)
import Html.App exposing (program)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Random
import Regex
import String


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


type alias WithParsedSignature a =
    { a
        | parsedSignature : TypeSignature.Signature
    }


type alias Function =
    WithParsedSignature Database.Function


parseSignatureCrashOnError :
    Database.Function
    -> TypeSignature.Signature
parseSignatureCrashOnError function =
    case
        (TypeSignature.parseSignature function.signature
            |> Maybe.map TypeSignature.normalizeSignature
        )
    of
        Just sig ->
            sig

        Nothing ->
            "Error parsing signature of "
                ++ function.name
                ++ ": "
                ++ function.signature
                |> Debug.crash


addParsedSignatureToFunction : Database.Function -> Function
addParsedSignatureToFunction function =
    { name = function.name
    , signature = function.signature
    , parsedSignature = parseSignatureCrashOnError function
    , documentation = function.documentation
    , declaration = function.declaration
    }


functions : List Function
functions =
    List.map addParsedSignatureToFunction Database.functions


functionCnt : Int
functionCnt =
    List.length functions


cmdGetRandomNumbers : Cmd Msg
cmdGetRandomNumbers =
    Random.list maxVisibleFunctions (Random.int 0 (functionCnt))
        |> Random.generate RandomNumbers


type alias Model =
    { query : String
    , querySigStr : String
    , searchResult : List ( Function, Float )
    }


defaultModel : Model
defaultModel =
    { query = ""
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
                    , querySigStr = ""
                    , searchResult = []
                  }
                , cmdGetRandomNumbers
                )
            else
                let
                    singletonSignatureToNothing sig =
                        if
                            String.contains "->"
                                (TypeSignature.showSignature True sig)
                        then
                            Just sig
                        else
                            Nothing

                    newQuerySig =
                        str
                            |> cleanFunctionSignature
                            |> TypeSignature.parseSignature
                            |> Maybe.map TypeSignature.normalizeSignature
                            |> \x -> Maybe.andThen x singletonSignatureToNothing

                    newQuerySigLower =
                        newQuerySig
                            |> Maybe.map
                                (TypeSignature.showSignature False
                                    >> String.toLower
                                )
                            |> (\x ->
                                    Maybe.andThen x
                                        (TypeSignature.parseSignature
                                            >> Maybe.map TypeSignature.normalizeSignature
                                        )
                               )

                    newQuerySigStr =
                        newQuerySig |> showMaybeSig
                in
                    ( { model
                        | query = str
                        , querySigStr = newQuerySigStr
                        , searchResult =
                            searchFunctions str
                                newQuerySig
                                newQuerySigLower
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
            TypeSignature.showSignature True s

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
                        "https://github.com/Dobiasd/FunctionalPlus/"
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
                    [ text
                        ("search by function name, docs or type signature,"
                            ++ " e.g. (vector<string>, string) -> string"
                        )
                    ]
              else
                div [ class "parsedsignature" ]
                    [ "as parsed type: "
                        ++ model.querySigStr
                        |> stringToCode "Haskell"
                    ]
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
            >> replaceInString "io" "Io"
            >> replaceInString "maybe" "Maybe"
            >> replaceInString "either" "Result"
            >> replaceInString "result" "Result"
            >> replaceInString "map" "Map"
            >> replaceInString "dict" "Map"
            >> replaceInString "set" "Int"
            >> replaceInString "string" "String"
            >> replaceInString "String" "[Char]"


searchFunctions :
    String
    -> Maybe TypeSignature.Signature
    -> Maybe TypeSignature.Signature
    -> List ( Function, Float )
searchFunctions query querySig querySigLower =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query querySig querySigLower f ))
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


typeRating :
    Float
    -> TypeSignature.Signature
    -> TypeSignature.Signature
    -> Float
typeRating weight query db =
    TypeSignature.functionCompatibility db query
        |> (*) weight


adjustQuery : String -> String
adjustQuery query =
    case query of
        "map" ->
            "transform"

        _ ->
            query


functionRating :
    String
    -> Maybe TypeSignature.Signature
    -> Maybe TypeSignature.Signature
    -> Function
    -> Float
functionRating queryOrig querySig querySigLower function =
    let
        query =
            queryOrig
                |> String.toLower
                |> adjustQuery

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
                        functionWordRating
                            (stringLengthFloat queryWord
                                / queryWordLengthSum
                            )
                            function
                            queryWord
                    )
                |> List.sum

        bestTypeRating =
            let
                maybeSigRating factor maybeSig =
                    case maybeSig of
                        Just sig ->
                            let
                                sigRating =
                                    typeRating factor
                                        sig
                                        function.parsedSignature

                                name_shortness_factor =
                                    120 / (120 + stringLengthFloat function.name)
                            in
                                sigRating * name_shortness_factor

                        _ ->
                            0
            in
                Basics.max (maybeSigRating 1000 querySig)
                    (maybeSigRating 400 querySigLower)

        functionNameLengthRating =
            -0.1 * stringLengthFloat function.name
    in
        wordRatingSum + bestTypeRating + functionNameLengthRating


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
            weight * Basics.max 0 (boolToNum 100 isSubStr - 5 * relLengthDiff)

        docRating =
            String.contains query (String.toLower function.documentation)
                |> boolToNum 40
    in
        nameRating + docRating


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
