module TypeSignature exposing (Signature, parseSignature, showSignature)

{-| This module provides the possibility to parse Haskell and Elm type signatures.
-}

-- where -- todo remove this comment

import Combine as C
import Combine.Char as CC
import Combine.Infix exposing (..)
import Combine.Num as CN
import List
import Maybe
import Result
import String


type Signature
    = Arrow Signature Signature
    | ListType Signature
      -- A Tuple with an empty List is the unit type.
    | Tuple (List Signature)
    | TypeConstructor String
    | TypeApplication Signature Signature
    | VariableType String


type alias ParseResult =
    ( Result (List String) Signature, C.Context )


isSignatureValid : Signature -> Bool
isSignatureValid sig =
    case sig of
        Arrow a b ->
            isSignatureValid a && isSignatureValid b

        TypeConstructor x ->
            True

        VariableType x ->
            True

        TypeApplication a b ->
            case a of
                TypeConstructor _ ->
                    isSignatureValid b

                TypeApplication _ _ ->
                    isSignatureValid a && isSignatureValid b

                _ ->
                    False

        ListType x ->
            isSignatureValid x

        Tuple xs ->
            List.all isSignatureValid xs


simplify : Signature -> Signature
simplify sig =
    case sig of
        Arrow a b ->
            Arrow (simplify a) (simplify b)

        ListType x ->
            ListType (simplify x)

        -- Extract value from possible 1-tuples.
        Tuple xs ->
            case xs of
                [ x ] ->
                    simplify x

                _ ->
                    sig

        TypeApplication a b ->
            TypeApplication (simplify a) (simplify b)

        _ ->
            sig

showSignature : Signature -> String
showSignature = showSignatureHelper False

addParenthesis : String -> String
addParenthesis x = "(" ++ x ++ ")"

showSignatureHelper : Bool -> Signature -> String
showSignatureHelper arrowsInParens sig =
    case sig of
        Arrow a b ->
            let
                optArrowParens =
                    if arrowsInParens then
                        addParenthesis
                    else
                        identity
            in
                showSignatureHelper True a ++ " -> " ++
                    showSignatureHelper False b
                    |> optArrowParens

        TypeConstructor x ->
            x

        VariableType x ->
            x

        TypeApplication a b ->
            showSignatureHelper False a ++ " " ++ showSignatureHelper True b

        ListType x ->
            "[" ++ showSignatureHelper False x ++ "]"

        Tuple xs ->
            String.join ", " (List.map (showSignatureHelper False) xs)
                |> addParenthesis


listParser : C.Parser Signature
listParser =
    C.brackets (C.rec <| \() -> signatureParser)
        |> C.map ListType


trimSpaces : C.Parser a -> C.Parser a
trimSpaces =
    let
        skipSpaces =
            C.skipMany <| C.choice [ CC.space, CC.tab ]
    in
        C.between skipSpaces skipSpaces


tupleParser : C.Parser Signature
tupleParser =
    let
        innerParser =
            C.sepBy (trimSpaces <| CC.char ',')
                (C.rec <| \() -> signatureParser)
                |> C.map Tuple
    in
        C.parens <| trimSpaces innerParser


arrowParser : C.Parser Signature
arrowParser =
    let
        arrowOp =
            Arrow <$ trimSpaces (C.string "->")
    in
        C.chainr (C.rec <| \() -> nonAppSignatureParser) arrowOp


typeApplicationParser : C.Parser Signature
typeApplicationParser =
    let
        typeApplyOp =
            TypeApplication <$ C.many1 CC.space
    in
        C.chainl (C.rec <| \() -> nonOpSignatureParser) typeApplyOp


typeStartsWithParser : C.Parser Char -> (String -> Signature) -> C.Parser Signature
typeStartsWithParser p tagger =
    [ p |> C.map (\x -> [ x ])
    , C.many <| C.choice [ CC.lower, CC.upper, CC.char '.' ]
    ]
        |> C.sequence
        |> C.map List.concat
        |> C.map (String.fromList >> tagger)


variableTypeParser : C.Parser Signature
variableTypeParser =
    typeStartsWithParser CC.lower VariableType


fixedTypeParser : C.Parser Signature
fixedTypeParser =
    typeStartsWithParser CC.upper TypeConstructor


nonOpSignatureParser : C.Parser Signature
nonOpSignatureParser =
    C.choice
        [ C.rec <| \() -> listParser
        , C.rec <| \() -> tupleParser
        , variableTypeParser
        , fixedTypeParser
        ]


nonAppSignatureParser : C.Parser Signature
nonAppSignatureParser =
    C.choice
        [ C.rec <| \() -> typeApplicationParser
        , C.rec <| \() -> nonOpSignatureParser
        ]


signatureParser : C.Parser Signature
signatureParser =
    C.choice
        [ C.rec <| \() -> arrowParser
        , nonAppSignatureParser
        ]
        |> C.map simplify


parseResultToMaybeSig : ParseResult -> Maybe Signature
parseResultToMaybeSig parseResult =
    case parseResult of
        ( Ok s, { input } ) ->
            if String.isEmpty input && isSignatureValid s then
                Maybe.Just s
            else
                Maybe.Nothing

        _ ->
            Maybe.Nothing


parseSignature : String -> Maybe Signature
parseSignature =
    C.parse signatureParser >> parseResultToMaybeSig
