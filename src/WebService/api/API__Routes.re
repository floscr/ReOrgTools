open Relude.Globals;
open ReludeURL;

let validateList = (fns: list('x => bool), xs: list('x)) =>
  if (List.length(xs) < List.length(fns)) {
    false;
  } else {
    xs
    |> List.findWithIndex((x, i) =>
         fns
         |> List.at(i)
         |> Option.filter(fn => fn(x))
         |> Option.isSome
         |> (x => !x)
       )
    |> Option.isNone;
  };

module Routes = {
  let fileUrl = (~workspaceIndex: int, ~id: string) => {j|/file/$workspaceIndex/$id|j};

  let openFile = (~workspaceIndex: int, ~id: string) =>
    ReasonReactRouter.replace(fileUrl(~workspaceIndex, ~id));

  let fileUrlValidations = [
    PathSegment.show >> String.eq("file"),
    PathSegment.show >> String.toInt >> Option.isSome,
    _ => true,
  ];

  let isFileRoute = url =>
    URI.parser
    |> ReludeParse.Parser.runParser(url)
    |> Result.toOption
    |> Option.map(URI.pathSegments)
    |> Option.filter(fileUrlValidations |> validateList)
    |> Option.isSome;
};
