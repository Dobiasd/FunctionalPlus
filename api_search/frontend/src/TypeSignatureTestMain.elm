module TypeSignatureTestMain exposing (..)

import TypeSignature
import Html exposing (..)
import Html.App exposing (beginnerProgram)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Result
import String


main =
    beginnerProgram
        { model = ""
        , update = update
        , view = view
        }


type alias Model =
    String


type Msg
    = UpdateStr String


update : Msg -> Model -> Model
update action model =
    case action of
        UpdateStr str ->
            str


showMaybeSig : Maybe TypeSignature.Signature -> String
showMaybeSig maybeSig =
    case maybeSig of
        Maybe.Just s ->
            TypeSignature.showSignature s

        Maybe.Nothing ->
            ""


view : Model -> Html Msg
view str =
    let
        parseResult =
            str |> TypeSignature.parseSignature

        maybeSignature =
            parseResult |> TypeSignature.parseResultToMaybeSig

        signatureString =
            showMaybeSig maybeSignature

        parsedAgainResult =
            signatureString
                |> TypeSignature.parseSignature

        maybeParsedAgainString =
            parsedAgainResult
                |> TypeSignature.parseResultToMaybeSig

        signatureAgainString =
            showMaybeSig maybeParsedAgainString

        isIdempotent =
            signatureString == signatureAgainString

        signatureStringToDisplay =
            if String.isEmpty signatureString then
                toString parseResult
            else
                signatureString

        signatureAgainStringToDisplay =
            if String.isEmpty signatureAgainString then
                toString parsedAgainResult
            else
                signatureAgainString
    in
        div []
            [ input
                [ placeholder "please enter type signature"
                , autofocus True
                , style [ ( "width", "500px" ) ]
                , onInput UpdateStr
                ]
                []
            , div [ style [ ( "margin", "10px" ) ] ]
                [ "parse result: " ++ (maybeSignature |> toString) |> text ]
            , div [] [ "as string: " ++ signatureStringToDisplay |> text ]
            , if not isIdempotent then
                div []
                    [
                    div [] [ "Error: Parsing was not isIdempotent!" |> text ]
                  , div [] [ signatureAgainString |> text ]
                  , div [] [ signatureAgainStringToDisplay |> text ]
                  ]
              else
                  div [] []
            ]
