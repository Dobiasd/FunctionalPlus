module FPlusApiSearch exposing (..)

import FPlusApiCommon exposing (..)
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


maxVisibleFunctions : Int
maxVisibleFunctions =
    20


initModelAndCommands : ( Model, Cmd Msg )
initModelAndCommands =
    ( defaultModel, Cmd.none )


type alias Model =
    { query : String
    , querySigStr : String
    , searchResult : List ( Function, Float )
    }


defaultModel : Model
defaultModel =
    { query = ""
    , querySigStr = ""
    , searchResult = []
    }


update : Msg -> Model -> ( Model, Cmd Msg )
update action model =
    case action of
        NoOp ->
            ( model, Cmd.none )

        UpdateQuery str ->
            if String.isEmpty str then
                ( { model
                    | query = str
                    , querySigStr = ""
                    , searchResult = []
                  }
                , Cmd.none
                )
            else
                let
                    singletonSignatureToNothing sig =
                        if
                            String.contains "->"
                                (TypeSignature.showSignature True sig)
                        then
                            Just sig
                        else
                            Nothing

                    newQuerySig =
                        str
                            |> cleanFunctionSignature
                            |> TypeSignature.parseSignature
                            |> Maybe.map TypeSignature.normalizeSignature
                            |> \x -> Maybe.andThen singletonSignatureToNothing x

                    newQuerySigLower =
                        newQuerySig
                            |> Maybe.map
                                (TypeSignature.showSignature False
                                    >> String.toLower
                                )
                            |> (\x ->
                                    Maybe.andThen
                                        (TypeSignature.parseSignature
                                            >> Maybe.map TypeSignature.normalizeSignature
                                        )
                                        x
                               )

                    newQuerySigStr =
                        newQuerySig |> showMaybeSig
                in
                    ( { model
                        | query = str
                        , querySigStr = newQuerySigStr
                        , searchResult =
                            searchFunctions str
                                newQuerySig
                                newQuerySigLower
                      }
                    , Cmd.none
                    )


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
                , input
                    [ placeholder "search query"
                    , autofocus True
                    , autocomplete True
                    , style [ ( "width", "500px" ) ]
                    , onInput UpdateQuery
                    ]
                    []
                , if String.isEmpty model.querySigStr then
                    div [ class "queryhelper" ]
                        [ text
                            ("search by function name, docs or type signature,"
                                ++ " e.g. (vector<string>, string) -> string"
                            )
                        ]
                  else
                    div [ class "parsedsignature" ]
                        [ "as parsed type: "
                            ++ model.querySigStr
                            |> stringToCode "Haskell"
                        ]
                , hr [] []
                , model.searchResult |> showFunctions
                , hr [] []
                , showFooter
                ]
        ]


showFooter : Html Msg
showFooter =
    footer [ class "footer" ]
        [ text "Copyright © 2016 Tobias Hermann. All rights reserved."
        ]


searchFunctions :
    String
    -> Maybe TypeSignature.Signature
    -> Maybe TypeSignature.Signature
    -> List ( Function, Float )
searchFunctions query querySig querySigLower =
    let
        ratedFunctions =
            functions
                |> List.map (\f -> ( f, functionRating query querySig querySigLower f ))
    in
        ratedFunctions
            |> List.filter (\( _, rating ) -> rating > 0)
            |> List.sortBy (\( _, rating ) -> 0 - rating)
            |> List.take maxVisibleFunctions


typeRating :
    Float
    -> TypeSignature.Signature
    -> TypeSignature.Signature
    -> Float
typeRating weight query db =
    TypeSignature.functionCompatibility db query
        |> (*) weight


adjustQuery : String -> String
adjustQuery query =
    case query of
        "map" ->
            "transform"

        _ ->
            query


functionRating :
    String
    -> Maybe TypeSignature.Signature
    -> Maybe TypeSignature.Signature
    -> Function
    -> Float
functionRating queryOrig querySig querySigLower function =
    let
        query =
            queryOrig
                |> String.toLower
                |> adjustQuery

        queryWords =
            String.words query

        stringLengthFloat =
            String.length >> toFloat

        queryWordLengthSum =
            queryWords
                |> List.map stringLengthFloat
                |> List.sum

        wordRatingSum =
            queryWords
                |> List.map
                    (\queryWord ->
                        functionWordRating
                            (stringLengthFloat queryWord
                                / queryWordLengthSum
                            )
                            function
                            queryWord
                    )
                |> List.sum
                |> \x ->
                    if maybeSigIsArrow querySig then
                        0
                    else
                        x

        bestTypeRating =
            let
                maybeSigRating factor maybeSig =
                    case maybeSig of
                        Just sig ->
                            let
                                sigRating =
                                    Basics.max
                                        (typeRating factor
                                            sig
                                            function.parsedSignature
                                        )
                                        (case function.parsedSignatureFwd of
                                            Maybe.Just psFwd ->
                                                typeRating factor
                                                    sig
                                                    psFwd

                                            Maybe.Nothing ->
                                                0
                                        )

                                name_shortness_factor =
                                    120 / (120 + stringLengthFloat function.name)
                            in
                                sigRating * name_shortness_factor

                        _ ->
                            0
            in
                Basics.max (maybeSigRating 1000 querySig)
                    (maybeSigRating 400 querySigLower)

        functionNameLengthRating =
            -0.1 * stringLengthFloat function.name
    in
        wordRatingSum + bestTypeRating + functionNameLengthRating


functionWordRating : Float -> Function -> String -> Float
functionWordRating weight function query =
    let
        isSubStr =
            String.contains query function.name

        queryLength =
            String.length query |> toFloat

        functionNameLength =
            String.length function.name |> toFloat

        lengthDiff =
            queryLength - functionNameLength |> abs

        queryAndFunctionNameMaxLength =
            Basics.max queryLength functionNameLength

        relLengthDiff =
            lengthDiff / queryAndFunctionNameMaxLength

        nameRating =
            weight * Basics.max 0 (boolToNum 100 isSubStr - 5 * relLengthDiff)

        docRating =
            String.contains query (String.toLower function.documentation)
                |> boolToNum 40
    in
        nameRating + docRating


showFunctions : List ( Function, Float ) -> Html Msg
showFunctions ratedFunctions =
    List.map showRatedFunction ratedFunctions
        |> List.intersperse (hr [] [])
        |> div [ class "functions" ]


ratingToHtml : Float -> Html Msg
ratingToHtml rating =
    "search rating: " ++ toString (round rating) |> docFromString


showRatedFunction : ( Function, Float ) -> Html Msg
showRatedFunction ( function, rating ) =
    let
        functionRating =
            div [ class "functionrating" ]
                [ rating
                    |> ratingToHtml
                ]
    in
        div [ class "function" ]
            ( showFunctionDivs function ++ [functionRating] )
