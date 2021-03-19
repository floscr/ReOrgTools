open Webapi.Url;
open Relude.Globals;

module Layouts = {
  type t =
    | Document
    | SimpleTodo
    | Kanban;

  let default = Document;

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
