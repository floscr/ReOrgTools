open Relude.Globals;
open ReactUtils;
open ReOrga;
open OrgDocument__Utils;

module Styles = {
  open Css;
  open FixedTheme;

  let playerThumbnail =
    style([
      borderRadius(BorderRadius.xsmall),
      overflow(hidden),
      width(px(640)),
      marginTop(Spacing.medium),
      cursor(pointer),
    ]);

  let player =
    style([
      borderRadius(BorderRadius.xsmall),
      overflow(hidden),
      marginTop(Spacing.medium),
    ]);
};

let renderAttachment = (~attachmentId=None, {value}) => {
  let dispatch = State.Store.useDispatch();

  let src =
    attachmentId
    |> Option.map(String.splitAt(2))
    |> Option.map(((a, b)) => [a, b])
    |> Option.map(xs => List.map(ReludeURL.PathSegment.make, xs))
    |> Option.map(xs =>
         ReludeURL.(
           Shared__Config.backendUrl(
             ~path=
               Path.make(
                 [
                   PathSegment.make("api"),
                   PathSegment.make("attachments"),
                   ...xs,
                 ]
                 |> List.append(PathSegment.make(value)),
               ),
             (),
           )
         )
       )
    |> Option.getOrElseLazy(_ =>
         ReludeURL.(
           Shared__Config.backendUrl(
             ~path=
               Path.make([
                 PathSegment.make("api"),
                 PathSegment.make("attachments"),
                 PathSegment.make(value),
               ]),
             (),
           )
         )
       )
    |> ReludeURL.URI.show;

  let onClick = () =>
    State.DialogsAction(
      State__Dialogs.OpenDialog(
        State__Dialogs.Lightbox({index: None, items: [|src|]}),
      ),
    );

  <img src onClick={_ => onClick() |> dispatch} />;
};

let renderUrlLinkSimple = (~description=?, value) =>
  <a href=value> {description |> Option.getOrElse(value) |> s} </a>;

let renderUrlLink = ({value, description}: ReOrga.link) =>
  <a href=value> {description |> Option.getOrElse(value) |> s} </a>;

module YoutubeComponent = {
  [@react.component]
  let make = (~link: ReOrga.link) => {
    let (showIframe, setShowIframe) = React.useState(() => false);

    let videoId =
      ReludeURL.(
        URI.parser
        |> ReludeParse.Parser.runParser(link.value)
        |> Result.toOption
        |> Option.flatMap(URI.getQueryParam(URI.QueryKey.make("v")))
        |> Option.flatMap(List.head)
        |> Option.map(QueryValue.show)
      );

    <>
      {renderUrlLink(link)}
      {videoId
       |> Option.map(videoId =>
            switch (showIframe) {
            | true =>
              <ReactYoutube
                videoId
                className=Styles.player
                onReady={e => e##target##playVideo()}
              />
            | _ =>
              <img
                src={j|https://img.youtube.com/vi/$videoId/maxresdefault.jpg|j}
                className=Styles.playerThumbnail
                onClick={_ => setShowIframe(_ => true)}
              />
            }
          )
       |> Option.getOrElse(React.null)}
    </>;
  };
};

type domLink =
  | Link(link)
  | Youtube(link)
  | Attachment(link);

let detectLinkType = (link: ReOrga.link) =>
  switch (link) {
  | {protocol} when Option.eqBy(String.eq, Some("attachment"), protocol) =>
    Attachment(link)

  | {value} when String.contains(~search="youtube.com", value) =>
    Youtube(link)

  | _ => Link(link)
  };

let renderLink = (~attachmentId=None, link) => {
  link
  |> detectLinkType
  |> (
    fun
    | Attachment(link) => renderAttachment(~attachmentId, link)
    | Youtube(link) => <YoutubeComponent link />
    | Link(link) => renderUrlLink(link)
  );
};

let renderPlainText = x =>
  switch (getItem(x)) {
  | PlainText({value, style}) =>
    let str = value |> s;
    switch (style) {
    | Plain => str
    | Bold => <b> str </b>
    | Italic => <i> str </i>
    | Underline => <u> str </u>
    | StrikeThrough => <del> str </del>
    | Verbatim => <code> str </code>
    };
  | _ => React.null
  };

let renderParagraphs = (~properties=None, xs) => {
  let atid = properties |> Option.flatMap(x => Js.Dict.get(x, "id"));

  xs
  |> Array.mapWithIndex((x, i) => {
       let key = string_of_int(i);
       switch (getItem(x)) {
       | PlainText(_) =>
         <React.Fragment key> {renderPlainText(x)} </React.Fragment>
       | Link(x) =>
         <React.Fragment key>
           {renderLink(~attachmentId=atid, x)}
         </React.Fragment>
       | _ => React.null
       };
     })
  |> React.array;
};
