module FPlusApiSearch (..) where

import Database exposing (Function, functions)
import Graphics.Element exposing (..)
import Graphics.Input.Field as Field
import Html as H
import Html.Attributes as HA
import Html.Events as HE
import Text as T
import Time


updateContentStr : Field.Content -> String -> Field.Content
updateContentStr cont str =
    { cont | string = str }


contentBox : Signal.Mailbox Field.Content
contentBox =
    Signal.mailbox Field.noContent


main : Signal Element
main =
    Signal.map scene contentBox.signal


scene : Field.Content -> Element
scene content =
    flow
        down
        [ H.input
            [ HA.placeholder "search query"
            , HA.autofocus True
              -- removed as a workaround for
              -- https://github.com/evancz/elm-html/issues/51
              --, HA.value content.string
            , HA.style [ ( "width", "500px" ) ]
            , HE.on
                "input"
                HE.targetValue
                (updateContentStr content
                    >> Signal.message contentBox.address
                )
            ]
            []
            |> H.toElement 500 100
        , searchFunction content.string |> showFunctions
        ]


searchFunction : String -> List Function
searchFunction query =
    functions


functionsSpacer : Element
functionsSpacer =
    spacer 32 32


functionSpacer : Element
functionSpacer =
    spacer 8 8


showFunctions : List Function -> Element
showFunctions functions =
    functions
        |> List.map showFunction
        |> List.intersperse functionsSpacer
        |> flow down


showFunction : Function -> Element
showFunction function =
    [ function.name
        ++ " : "
        ++ function.signature
        |> T.fromString
        |> T.bold
    , function.documentation |> T.fromString
    , function.declaration |> T.fromString
    ]
        |> List.map (T.monospace >> leftAligned)
        |> List.intersperse functionSpacer
        |> flow down
