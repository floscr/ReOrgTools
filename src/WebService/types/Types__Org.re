module Layout = {
  type t =
    | Document
    | SimpleTodo
    | Kanban
    | Calendar;

  let default = Document;
  let options = [|Document, SimpleTodo, Kanban, Calendar|];

  let fromString =
    fun
    | "Document" => Document
    | "Simple Todo" => SimpleTodo
    | "Kanban" => Kanban
    | "Calendar" => Calendar
    | _ => default;

  let toString =
    fun
    | Document => "Document"
    | SimpleTodo => "Simple Todo"
    | Kanban => "Kanban"
    | Calendar => "Calendar";

  let reactSelectOptions = options |> Array.map(toString);
};
