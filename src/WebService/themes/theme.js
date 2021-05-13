// https://components.ai/theme/1wVNXfB1th776n3jQgSe
export const theme = {
  breakpoints: ["32em", "64em"],
  description:
    "Color system based on shadows and tints. The 6th step in each color scale is accessible with both white and black.",
  space: [
    "0px",
    "2px",
    "4px",
    "8px",
    "16px",
    "32px",
    "64px",
    "128px",
    "256px",
    "512px",
  ],
  fonts: { heading: '"IBM Plex Sans", sans-serif' },
  textShadows: {
    0: "35px -2px 34px hsla(145.85,29.15%,43.73%,0.139)",
    1: "38px 43px 91px hsla(353.06,68.37%,57.84%,0.271)",
  },
  letterSpacings: { tracked: "0.1em" },
  text: {
    heading: {
      "font-family": "heading",
      "font-weight": 200,
      "line-height": "1",
      "letter-spacing": "0.1em",
      "font-size": "0.875rem",
    },
  },
  fontSizes: [
    "0.75rem",
    "0.875rem",
    "1rem",
    "1.25rem",
    "1.5rem",
    "2rem",
    "3rem",
    "4rem",
  ],
  borderStyles: ["none", "solid"],
  borderRadius: ["0px", "5px", "9px", "9999px"],
  name: "Test",
  gradients: {
    0:
      "radial-gradient(circle at -159% 117%, hsl(17.14,100%,72.55%) 24%, hsl(33,66.67%,23.53%) 95%)",
    1:
      "radial-gradient(circle at 157% 147%, hsl(33,66.67%,23.53%) 75%, hsl(228,60%,95.1%) 82%)",
    2:
      "radial-gradient(circle at 140% -120%, hsl(245.25,64.52%,75.69%) 21%, hsl(196,50.24%,59.02%) 94%)",
    3:
      "radial-gradient(circle at 167% -188%, hsl(33.85,77.69%,50.78%) 31%, hsl(337.99,57.2%,52.35%) 34%)",
    4:
      "linear-gradient(359deg, hsl(220,1.28%,46.08%) 19%, hsl(237.98,62.3%,62.55%) 3%)",
    5:
      "radial-gradient(circle at 181% -150%, hsl(169.79,50%,63.14%) 25%, hsl(240,0.78%,74.71%) 45%)",
  },
  lineHeights: { solid: 1, heading: 1.25, body: 1.5 },
  boxShadows: {
    1:
      "0 1px 2px rgba(0,0,0,0.07), 0 2px 4px rgba(0,0,0,0.07), 0 4px 8px rgba(0,0,0,0.07), 0 8px 16px rgba(0,0,0,0.07), 0 16px 32px rgba(0,0,0,0.07), 0 32px 64px rgba(0,0,0,0.07)",
    2:
      "0 1px 1px rgba(0,0,0,0.25), 0 2px 2px rgba(0,0,0,0.20), 0 4px 4px rgba(0,0,0,0.15), 0 8px 8px rgba(0,0,0,0.10), 0 16px 16px rgba(0,0,0,0.05)",
    3:
      "0 1px 1px rgba(0,0,0,0.125), 0 2px 2px rgba(0,0,0,0.125), 0 4px 4px rgba(0,0,0,0.125), 0 8px 8px rgba(0,0,0,0.125), 0 16px 16px rgba(0,0,0,0.125)",
    4: "0 14px 28px rgba(0,0,0,0.25), 0 10px 10px rgba(0,0,0,0.22)",
    5:
      "0 1px 1px rgba(0,0,0,0.15), 0 2px 2px rgba(0,0,0,0.15), 0 4px 4px rgba(0,0,0,0.15), 0 8px 8px rgba(0,0,0,0.15)",
    6: "0 14px 28px rgba(0,0,0,0.25), 0 10px 10px rgba(0,0,0,0.22)",
  },
  colors: {
    baseGray: [
      "rgb(29, 27, 27)",
      "rgb(58, 56, 56)",
      "rgb(90, 88, 88)",
      "rgb(122, 120, 120)",
      "rgb(153, 151, 151)",
      "rgb(181, 179, 179)",
      "rgb(205, 203, 203)",
      "rgb(221, 219, 219)",
      "rgb(232, 231, 231)",
      "rgb(240, 239, 239)",
      "rgb(245, 244, 244)",
      "rgb(248, 247, 247)",
      "rgb(249, 248, 248)",
      "rgb(249, 248, 248)",
      "rgb(248, 248, 248)",
    ],
    base: ["rgba(0, 0, 0, 1)", "rgba(255, 255, 255, 1)"],
    blue: [
      "rgb(30, 48, 67)",
      "rgb(39, 72, 102)",
      "rgb(47, 97, 139)",
      "rgb(54, 123, 179)",
      "rgb(61, 150, 220)",
      "rgb(104, 167, 227)",
      "rgb(139, 184, 233)",
      "rgb(170, 202, 239)",
      "rgb(199, 220, 244)",
      "rgb(228, 238, 250)",
    ],
    modes: {
      "minimal-dark": {
        background: "rgb(43, 42, 41)",
        text: "rgb(235, 235, 235)",
        border: "rgb(228, 238, 250)",
        primary: "hsl(210,9.09%,95.69%)",
        accent: "rgb(95, 93, 92)",
        muted: "rgb(146, 142, 141)",
      },
      dimmed: {
        background: "hsl(240,1.08%,18.24%)",
        text: "hsl(240,0.78%,74.71%)",
        border: "hsla(301, 96%, 49%, 1)",
        primary: "hsl(220,1.28%,46.08%)",
        accent: "hsl(180,0.73%,26.86%)",
        muted: "hsl(240,1.11%,64.71%)",
      },
      default: {
        background: "rgba(255, 255, 255, 1)",
        text: "rgb(58, 56, 56)",
        border: "rgba(255, 255, 255, 1)",
        primary: "rgb(54, 123, 179)",
        accent: "rgb(61, 150, 220)",
        muted: "rgb(95, 92, 92)",
      },
    },
  },
  borderWidths: ["0px", "1px", "2px", "4px"],
}
