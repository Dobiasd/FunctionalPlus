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
    , searchResult : List Function
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
        [ input
            [ placeholder "search query"
            , autofocus True
            , style [ ( "width", "500px" ) ]
            , on "input" targetValue (Signal.message address << UpdateQuery)
            ]
            []
        , hr [] []
        , model.searchResult |> showFunctions
        ]


searchFunctions : String -> List Function
searchFunctions query =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( functionRating query f, f ))
    in
        ratedFunctions
            |> List.sortBy fst
            |> List.map snd
            |> List.take 20


functionRating : String -> Function -> Float
functionRating query function =
    StringDistance.sift3Distance query function.name



--StringDistance.lcs (String.toList query) (String.toList function.name) |> List.length |> toFloat


showFunctions : List Function -> Html
showFunctions functions =
    List.map showFunction functions
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


showFunction : Function -> Html
showFunction function =
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
    in
        div
            [ class "function" ]
            [ functionNameAndSig
            , functionDocumentation
            , functionDeclaration
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
