open Relude.Globals;

let getFiles = () => {
  Config.orgDir
  |> Node.Fs.readdirSync
  |> Array.filter(x => Filename.extension(x) |> String.eq(".org"));
};
