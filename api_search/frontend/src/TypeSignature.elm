module TypeSignature exposing (..)

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
    | VariableType String
    | TypeConstructor String
    | TypeApplication Signature Signature
    | ListType Signature
    | Tuple (List Signature)


simplify : Signature -> Signature
simplify sig =
    case sig of
        Tuple xs ->
            case xs of
                [ x ] ->
                    simplify x

                _ ->
                    sig

        Arrow a b ->
            Arrow (simplify a) (simplify b)

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

        TypeApplication x y ->
            showSignature x ++ " " ++ showSignature y

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


singletonList : a -> List a
singletonList x =
    [ x ]


signatureInParensParser : C.Parser Signature
signatureInParensParser =
    C.parens (C.rec <| \() -> signatureParser)


listParser : C.Parser Signature
listParser =
    C.between (trimSpaces <| C.string "[")
        (trimSpaces <| C.string "]")
        (C.rec <| \() -> signatureParser)
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
            C.sepBy1 (trimSpaces <| CC.char ',') (C.rec <| \() -> signatureParser)
                |> C.map Tuple
    in
        C.between (trimSpaces <| C.string "(")
            (trimSpaces <| C.string ")")
            innerParser


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
    [ p |> C.map singletonList
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



-- todo raus


try : C.Parser res -> C.Parser res
try p =
    let
        f c =
            case C.parse p c.input of
                ( Result.Ok r, cRemaining ) ->
                    ( Result.Ok r, cRemaining )

                ( Result.Err e, cRemaining ) ->
                    ( Result.Err e, c )
    in
        C.primitive f


nonOpSignatureParser : C.Parser Signature
nonOpSignatureParser =
    C.choice
        [ C.rec <| \() -> listParser
        , C.rec <| \() -> tupleParser
        , C.rec <| \() -> signatureInParensParser
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


parseResultToMaybeSig : ( Result (List String) Signature, C.Context ) -> Maybe Signature
parseResultToMaybeSig parseResult =
    case parseResult of
        ( Ok s, { input } ) ->
            if String.isEmpty input then
                Maybe.Just s
            else
                Maybe.Nothing

        _ ->
            Maybe.Nothing


parseSignature : String -> Maybe Signature
parseSignature =
    C.parse signatureParser >> parseResultToMaybeSig



-- todo: empty type: ()
