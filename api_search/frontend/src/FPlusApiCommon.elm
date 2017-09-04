module FPlusApiCommon exposing (..)

import Database
import TypeSignature
import Html exposing (..)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Regex
import String


type Msg
    = NoOp
    | UpdateQuery String


type alias WithParsedSignature a =
    { a
        | parsedSignature : TypeSignature.Signature
        , signatureFwd : Maybe String
        , parsedSignatureFwd : Maybe TypeSignature.Signature
        , signatureFwdFlip : Maybe String
        , parsedSignatureFwdFlip : Maybe TypeSignature.Signature
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

hasFwdFlipSignature : String -> Bool
hasFwdFlipSignature documentation =
    String.contains "fwd bind count: 1" documentation

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

        parsedSigFwdFlip =
            if hasFwdFlipSignature function.documentation then
                parsedSig
                    |> TypeSignature.curry1Flip
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
        , parsedSignatureFwdFlip =
            parsedSigFwdFlip
                |> Maybe.map TypeSignature.normalizeSignature
        , signatureFwdFlip =
            parsedSigFwdFlip
                |> Maybe.map (TypeSignature.showSignature True)
        , documentation = removeFwdBindCount function.documentation
        , declaration = function.declaration
        }


functions : List Function
functions =
    List.map addParsedSignatureToFunction Database.functions


showMaybeSig : Maybe TypeSignature.Signature -> String
showMaybeSig maybeSig =
    case maybeSig of
        Maybe.Just s ->
            TypeSignature.showSignature True s

        Maybe.Nothing ->
            ""


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


maybeSigIsArrow : Maybe TypeSignature.Signature -> Bool
maybeSigIsArrow maybeSig =
    case maybeSig of
        Maybe.Just sig ->
            TypeSignature.sigIsArrow sig

        _ ->
            False


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


showFunctionDivs : Function -> List (Html Msg)
showFunctionDivs function =
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

        functionNameAndSigFwdFlip =
            case function.signatureFwdFlip of
                Maybe.Just sigFwdFlip ->
                    div [ class "functionnameandsig" ]
                        [ "fwd::flip::"
                            ++ function.name
                            ++ " : "
                            ++ sigFwdFlip
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
        [ functionNameAndSig
        , functionNameAndSigFwd
        , functionNameAndSigFwdFlip
        , functionDocumentation
        , functionDeclaration
        ]
