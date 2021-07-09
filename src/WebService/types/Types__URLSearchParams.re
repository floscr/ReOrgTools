open Webapi.Url;
open Relude.Globals;
open Types__Layouts;

type t = {
  narrowToHeader: option(string),
  layoutType: Layout.t,
};

let empty = {narrowToHeader: None, layoutType: Layout.default};

let make = (params: Webapi__Url.URLSearchParams.t) => {
  narrowToHeader:
    params
    |> URLSearchParams.get("header")
    |> Option.flatMap(String.toNonWhitespace),
  layoutType:
    params
    |> URLSearchParams.get("layoutType")
    |> Option.map(Layout.fromString)
    |> Option.getOrElse(Layout.default),
};
