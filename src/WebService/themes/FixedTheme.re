module Fonts = {
  open Css;

  let small = rem(0.9);
  let regular = rem(1.);
  let large = rem(1.2);
};

module Spacing = {
  open Css;

  let mediumValue = 0.7;
  let negativeMediumValue = (-0.7);
  let smallValue = 0.55;
  let negativeSmallValue = (-0.55);

  let xxlarge = rem(1.75);
  let xlarge = rem(1.5);
  let large = rem(1.);
  let negativeMedium = rem(negativeMediumValue);
  let medium = rem(mediumValue);
  let small = rem(smallValue);
  let negativeSmall = rem(negativeSmallValue);
  let xsmall = rem(0.45);
  let xxsmall = rem(0.35);

  let scrollbarWidth = px(14);
};

module SpacingNew = {
  open Css;

  /*
   Source: https://justrem.xyz/
   Base: 16px
   */
  let px10 = rem(0.625);
  let px15 = rem(0.938);
  let px20 = rem(1.25);
  let px25 = rem(1.563);
  let px30 = rem(1.875);
  let px35 = rem(2.188);
  let px40 = rem(2.5);
  let px45 = rem(2.813);
  let px50 = rem(3.125);
  let px55 = rem(3.438);
  let px60 = rem(3.75);
  let px65 = rem(4.063);
  let px70 = rem(4.375);
  let px75 = rem(4.688);
  let px80 = rem(5.);
  let px85 = rem(5.313);
  let px90 = rem(5.625);
};

module BorderRadius = {
  open Css;

  let xsmall = px(4);
  let small = px(5);
};

module Timing = {
  open Css;
  let hoverShort = 150;
};
