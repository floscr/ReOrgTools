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

let validateUrl = (~url: string, ~validations: list('x => bool)) =>
  URI.parser
  |> ReludeParse.Parser.runParser(url)
  |> Result.toOption
  |> Option.map(URI.pathSegments)
  |> Option.filter(validations |> validateList)
  |> Option.isSome;

module Routes = {
  let fileUrl = (~workspaceIndex: int, ~id: string) => {j|/file/$workspaceIndex/$id|j};

  let openFile = (~workspaceIndex: int, ~id: string) =>
    ReasonReactRouter.replace(fileUrl(~workspaceIndex, ~id));

  let isFileRoute = url =>
    validateUrl(
      ~url,
      ~validations=[
        PathSegment.show >> String.eq("file"),
        PathSegment.show >> String.toInt >> Option.isSome,
        _ => true,
      ],
    );
};
