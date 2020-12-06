module FPlusApiExplore exposing (..)

import FPlusApiCommon exposing (..)
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


type alias Model =
    { allFunctions : List Function
    }


defaultModel : Model
defaultModel =
    { allFunctions = functions
    }


update : Msg -> Model -> ( Model, Cmd Msg )
update action model =
    case action of
        NoOp ->
            ( model, Cmd.none )

        UpdateQuery str ->
            ( model, Cmd.none )


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


showNonRatedFunction : Function -> Html Msg
showNonRatedFunction function =
    div [ class "function" ] ( showFunctionDivs function )


showFunctions : List Function -> Html Msg
showFunctions ratedFunctions =
    List.map showNonRatedFunction ratedFunctions
        |> List.intersperse (hr [] [])
        |> div [ class "functions" ]
