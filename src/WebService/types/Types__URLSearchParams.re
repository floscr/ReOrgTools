open Webapi.Url;
open Relude.Globals;

module Layouts = {
  type t =
    | Document
    | SimpleTodo
    | Kanban;

  let default = Document;
  let options = [|Document, SimpleTodo, Kanban|];

  // String Encoding (properties)

  let fromString =
    fun
    | "DOCUMENT" => Document
    | "SIMPLE_TODO" => SimpleTodo
    | "KANBAN" => Kanban
    | _ => default;

  let toString =
    fun
    | Document => "DOCUMENT"
    | SimpleTodo => "SIMPLE_TODO"
    | Kanban => "KANBAN";

  let toReactSelect =
    fun
    | Document => ReactSelect.{label: "Document", value: "DOCUMENT"}
    | SimpleTodo => ReactSelect.{label: "Todo", value: "SIMPLE_TODO"}
    | Kanban => ReactSelect.{label: "Kanban", value: "KANBAN"};

  let reactSelectOptions = options |> Array.map(toReactSelect);
};

type t = {
  narrowToHeader: option(string),
  layoutType: Layouts.t,
};

let make = (params: Webapi__Url.URLSearchParams.t) => {
  narrowToHeader:
    params
    |> URLSearchParams.get("header")
    |> Option.flatMap(String.toNonWhitespace),
  layoutType:
    params
    |> URLSearchParams.get("layoutType")
    |> Option.map(Layouts.fromString)
    |> Option.getOrElse(Layouts.default),
};
