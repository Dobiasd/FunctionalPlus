module TypeSignature exposing (Signature, parseSignature, ParseResult, showSignature, parseResultToMaybeSig)

{-| This module provides the possibility to parse Haskell and Elm type signatures.
-}

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
showSignature sig =
    case sig of
        Arrow a b ->
            "(" ++ showSignature a ++ " -> " ++ showSignature b ++ ")"

        TypeConstructor x ->
            x

        VariableType x ->
            x

        TypeApplication a b ->
            "(" ++ showSignature a ++ " " ++ showSignature b ++ ")"

        ListType x ->
            "[" ++ showSignature x ++ "]"

        Tuple xs ->
            let
                str =
                    String.join "," (List.map showSignature xs)
            in
                if List.length xs == 1 then
                    str
                else
                    "(" ++ str ++ ")"


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
    , C.many <| C.choice [ CC.lower, CC.upper ]
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
            if String.isEmpty input then
                Maybe.Just s
            else
                Maybe.Nothing

        _ ->
            Maybe.Nothing


parseSignature : String -> ParseResult
parseSignature =
    C.parse signatureParser
