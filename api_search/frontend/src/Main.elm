module FPlusApiSearch (..) where

import Database exposing (Function, functions)
import Html exposing (..)
import Html.Attributes exposing (..)
import Html.Events exposing (..)
import Markdown
import Signal exposing (Signal, Address)
import String
import StringDistance


type alias Model =
    { query : String
    , searchResult : List ( Function, Float )
    }


emptyModel : Model
emptyModel =
    { query = ""
    , searchResult = []
    }


type Action
    = NoOp
    | UpdateQuery String


update : Action -> Model -> Model
update action model =
    case action of
        NoOp ->
            model

        UpdateQuery str ->
            { model
                | query = str
                , searchResult = searchFunctions str
            }


view : Address Action -> Model -> Html
view address model =
    div
        [ class "main" ]
        [ img [ src "fplus.png" ] []
        , hr [] []
        , input
            [ placeholder "search query"
            , autofocus True
            , style [ ( "width", "500px" ) ]
            , on "input" targetValue (Signal.message address << UpdateQuery)
            ]
            []
        , hr [] []
        , model.searchResult |> showFunctions
        , hr [] []
        , showFooter
        ]


showFooter : Html
showFooter =
    footer
        [ class "footer" ]
        [ text "Copyright © 2016 Tobias Hermann. All rights reserved."
        ]


searchFunctions : String -> List ( Function, Float )
searchFunctions query =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query f ))
    in
        ratedFunctions
            |> List.sortBy (\( _, rating ) -> 0 - rating)
            |> List.take 20
            |> List.filter (\( _, rating ) -> rating > 0)


functionRating : String -> Function -> Float
functionRating query function =
    String.words query |> List.map (functionWordRating function) |> List.sum


functionWordRating : Function -> String -> Float
functionWordRating function query =
    --StringDistance.sift3Distance query function.name
    let
        boolToNum value b =
            if b then
                value
            else
                0

        -- todo: rate type
        isSubStr = String.contains query function.name

        lengthDiff = String.length query - String.length function.name |> abs |> toFloat

        nameRating = Basics.max 0 (boolToNum 1000 isSubStr - 100 * lengthDiff)

        docRating = String.contains query function.documentation |> boolToNum 10
    in
        nameRating + docRating



--StringDistance.lcs (String.toList query) (String.toList function.name) |> List.length |> toFloat


showFunctions : List ( Function, Float ) -> Html
showFunctions ratedFunctions =
    List.map showRatedFunction ratedFunctions
        |> List.intersperse (hr [] [])
        |> div [ class "functions" ]


stringToCode : String -> String -> Html
stringToCode language str =
    let
        defOpts = Markdown.defaultOptions

        options = { defOpts | defaultHighlighting = Just language }

        taggedStr = "```\n" ++ str ++ "\n```"
    in
        Markdown.toHtmlWith options taggedStr


stringToDoc : String -> Html
stringToDoc str =
    let
        taggedStr = "```" ++ str ++ "```"
    in
        Markdown.toHtmlWith Markdown.defaultOptions taggedStr


ratingToHtml : Float -> Html
ratingToHtml rating =
    "search rating: " ++ toString rating |> stringToDoc


showRatedFunction : ( Function, Float ) -> Html
showRatedFunction ( function, rating ) =
    let
        functionNameAndSig =
            div
                [ class "functionnameandsig" ]
                [ function.name
                    ++ " : "
                    ++ function.signature
                    |> stringToCode "elm"
                ]

        functionDocumentation =
            div
                [ class "functiondoc" ]
                [ function.documentation
                    |> stringToDoc
                ]

        functionDeclaration =
            div
                [ class "functiondecl" ]
                [ function.declaration
                    |> stringToCode "cpp"
                ]

        functionRating =
            div
                [ class "functionrating" ]
                [ rating
                    |> ratingToHtml
                ]
    in
        div
            [ class "function" ]
            [ functionNameAndSig
            , functionDocumentation
            , functionDeclaration
            , functionRating
            ]


main : Signal Html
main =
    Signal.map (view actions.address) model


model : Signal Model
model =
    Signal.foldp update emptyModel actions.signal


actions : Signal.Mailbox Action
actions =
    Signal.mailbox NoOp
