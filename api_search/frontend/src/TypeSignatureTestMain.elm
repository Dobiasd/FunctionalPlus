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


showMaybeSig maybeSig =
    case maybeSig of
        Maybe.Just s ->
            TypeSignature.showSignature s

        Maybe.Nothing ->
            ""


view : Model -> Html Msg
view str =
    let
        maybeSignature =
            str
                |> TypeSignature.parseSignature

        signatureString =
            showMaybeSig maybeSignature

        maybeParsedAgainString =
            signatureString
                |> TypeSignature.parseSignature

        signatureAgainString =
            showMaybeSig maybeParsedAgainString

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
            , div [] [ "as string: " ++ signatureString |> text ]
            , if not isIdempotent then
                div []
                    [ div [] [ signatureAgainString |> text ]
                    , div []
                        [ "isIdempotent (should always be True): "
                            ++ toString isIdempotent
                            |> text
                        ]
                    ]
              else
                div [] []
            ]
