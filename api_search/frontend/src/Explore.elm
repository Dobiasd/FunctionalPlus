module FPlusApiExplore exposing (..)

import Database
import TypeSignature
import Html exposing (..)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Regex
import String


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


type alias WithParsedSignature a =
    { a
        | parsedSignature : TypeSignature.Signature
        , signatureFwd : Maybe String
        , parsedSignatureFwd : Maybe TypeSignature.Signature
    }


type alias Function =
    WithParsedSignature Database.Function


parseSignatureCrashOnError :
    Database.Function
    -> TypeSignature.Signature
parseSignatureCrashOnError function =
    case
        TypeSignature.parseSignature function.signature
    of
        Just sig ->
            sig

        Nothing ->
            "Error parsing signature of "
                ++ function.name
                ++ ": "
                ++ function.signature
                |> Debug.crash


hasFwdSignature : String -> Bool
hasFwdSignature documentation =
    String.contains "fwd bind count" documentation


removeFwdBindCount : String -> String
removeFwdBindCount documentation =
    documentation
        |> String.lines
        |> List.filter (\x -> String.contains "fwd bind count" x |> not)
        |> String.join "\n"


addParsedSignatureToFunction : Database.Function -> Function
addParsedSignatureToFunction function =
    let
        parsedSig =
            parseSignatureCrashOnError function

        parsedSigFwd =
            if hasFwdSignature function.documentation then
                parsedSig
                    |> TypeSignature.curry1
                    |> Maybe.Just
            else
                Maybe.Nothing
    in
        { name = function.name
        , signature = function.signature
        , parsedSignature = parsedSig |> TypeSignature.normalizeSignature
        , parsedSignatureFwd =
            parsedSigFwd
                |> Maybe.map TypeSignature.normalizeSignature
        , signatureFwd =
            parsedSigFwd
                |> Maybe.map (TypeSignature.showSignature True)
        , documentation = removeFwdBindCount function.documentation
        , declaration = function.declaration
        }


functions : List Function
functions =
    List.map addParsedSignatureToFunction Database.functions


type alias Model =
    { allFunctions : List Function
    }


defaultModel : Model
defaultModel =
    { allFunctions = functions
    }


type Msg
    = NoOp


update : Msg -> Model -> ( Model, Cmd Msg )
update action model =
    case action of
        NoOp ->
            ( model, Cmd.none )


showMaybeSig : Maybe TypeSignature.Signature -> String
showMaybeSig maybeSig =
    case maybeSig of
        Maybe.Just s ->
            TypeSignature.showSignature True s

        Maybe.Nothing ->
            ""


view : Model -> Html Msg
view model =
    div [ class "mainwrapper" ]
        [ let
            url =
                "https://github.com/Dobiasd/FunctionalPlus/"
          in
            div [ class "main" ]
                [ div [ class "githublink" ]
                    [ a [ href url ]
                        [ img [ class "logo", src "fplus.png" ] []
                        ]
                    , p [] [ a [ href url ] [ text url ] ]
                    ]
                , hr [] []
                , model.allFunctions |> showFunctions
                , hr [] []
                , showFooter
                ]
        ]


showFooter : Html Msg
showFooter =
    footer [ class "footer" ]
        [ text "Copyright © 2017 Tobias Hermann. All rights reserved."
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
            >> replaceInString "set" "Set"
            >> replaceInString "string" "String"
            >> replaceInString "String" "[Char]"


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


maybeSigIsArrow : Maybe TypeSignature.Signature -> Bool
maybeSigIsArrow maybeSig =
    case maybeSig of
        Maybe.Just sig ->
            TypeSignature.sigIsArrow sig

        _ ->
            False


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
                |> \x ->
                    if maybeSigIsArrow querySig then
                        0
                    else
                        x

        bestTypeRating =
            let
                maybeSigRating factor maybeSig =
                    case maybeSig of
                        Just sig ->
                            let
                                sigRating =
                                    Basics.max
                                        (typeRating factor
                                            sig
                                            function.parsedSignature
                                        )
                                        (case function.parsedSignatureFwd of
                                            Maybe.Just psFwd ->
                                                typeRating factor
                                                    sig
                                                    psFwd

                                            Maybe.Nothing ->
                                                0
                                        )

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


showFunctions : List Function -> Html Msg
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


showRatedFunction : Function -> Html Msg
showRatedFunction function =
    let
        functionNameAndSig =
            div [ class "functionnameandsig" ]
                [ function.name
                    ++ " : "
                    ++ function.signature
                    |> stringToCode "haskell"
                ]

        functionNameAndSigFwd =
            case function.signatureFwd of
                Maybe.Just sigFwd ->
                    div [ class "functionnameandsig" ]
                        [ "fwd::"
                            ++ function.name
                            ++ " : "
                            ++ sigFwd
                            |> stringToCode "haskell"
                        ]

                Maybe.Nothing ->
                    div [] []

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
    in
        div [ class "function" ]
            [ functionNameAndSig
            , functionNameAndSigFwd
            , functionDocumentation
            , functionDeclaration
            ]
