module Layout = {
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
