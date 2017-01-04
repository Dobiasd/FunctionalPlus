module TypeSignatureTestMain exposing (..)

import TypeSignature
import Html exposing (..)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Maybe
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
            TypeSignature.showSignature True s

        Maybe.Nothing ->
            ""


view : Model -> Html Msg
view str =
    let
        maybeSignature =
            str |> TypeSignature.parseSignature

        maybeSignatureNormalized =
            maybeSignature |> Maybe.map TypeSignature.normalizeSignature

        signatureString =
            maybeSignatureNormalized |> showMaybeSig

        maybeParsedAgainSignature =
            signatureString
                |> TypeSignature.parseSignature

        signatureAgainString =
            showMaybeSig maybeParsedAgainSignature

        isIdempotent =
            signatureString == signatureAgainString
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
            , div [ style [ ( "margin", "10px" ) ] ]
                [ "parse result: "
                    ++ (maybeSignatureNormalized |> toString)
                    |> text
                ]
            , div [] [ "as string: " ++ signatureString |> text ]
            , if not isIdempotent then
                div []
                    [ div [] [ "Error: Parsing was not isIdempotent!" |> text ]
                    , div [] [ signatureAgainString |> text ]
                    ]
              else
                div [] []
            ]
