open Webapi.Url;
open Relude.Globals;

module Layouts = {
  type t =
    | Document
    | SimpleTodo
    | Kanban;

  let default = Document;
  let options = [|Document, SimpleTodo, Kanban|];

  let fromString =
    fun
    | "Document" => Document
    | "Simple Todo" => SimpleTodo
    | "Kanban" => Kanban
    | _ => default;

  let toString =
    fun
    | Document => "Document"
    | SimpleTodo => "Simple Todo"
    | Kanban => "Kanban";

  let reactSelectOptions = options |> Array.map(toString);
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
