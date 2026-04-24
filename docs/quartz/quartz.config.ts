import { QuartzConfig } from "./quartz/cfg"
import * as Plugin from "./quartz/plugins"

const config: QuartzConfig = {
	configuration: {
		pageTitle: "Kiwi Docs",
		enableSPA: true,
		enablePopovers: true,
		enableSitemap: true,
		enableRSS: false,
		analytics: null,
		baseUrl: "mevex.github.io/kiwi-game-engine",
		ignorePatterns: [".obsidian", "drafts", "templates"],
		defaultDateType: "modified",
		theme: {
			fontOrigin: "googleFonts",
			cdnCaching: true,
			typography: {
				title: "Outfit",
				header: {
					name: "Outfit",
					weights: [400, 800],
					includeItalic: false,
				},
				body: {
					name: "Outfit",
					weights: [400, 800],
					includeItalic: false,
				},
				code: "JetBrains Mono",
			},
			colors: {
				lightMode: {
					light: "#f2f2f2",
					lightgray: "#ccc",
					gray: "#b8b8b8",
					darkgray: "#404040",
					dark: "#303030",
					secondary: "#88ad00",
					tertiary: "#a8d500",
					highlight: "rgba(255, 255, 255, 0.8)",
					textHighlight: "rgba(168, 213, 0, 0.8)",
				},
				darkMode: {
					light: "#161616",
					lightgray: "#704d25",
					gray: "#646464",
					darkgray: "#d4d4d4",
					dark: "#ffffff",
					secondary: "#a8d500",
					tertiary: "rgba(168, 213, 0, 0.8)",
					highlight: "rgba(168, 168, 168, 0.15)",
					textHighlight: "rgba(168, 213, 0, 0.8)",
				}
			}
		},
	},
	plugins: {
		transformers: [
			Plugin.CrawlLinks({
				markdownLinkResolution: "shortest",
				openLinksInNewTab: true,
				lazyLoad: true,
			}),
			Plugin.FrontMatter(),
			Plugin.CreatedModifiedDate({
				priority: ["frontmatter", "git", "filesystem"],
			}),
			Plugin.SyntaxHighlighting(),
			Plugin.ObsidianFlavoredMarkdown({
				disableBrokenWikilinks: true
			}),
			Plugin.GitHubFlavoredMarkdown(),
			Plugin.TableOfContents(),
			Plugin.Description(),
			Plugin.Latex(),
		],
		filters: [
			Plugin.RemoveDrafts()
		],
		emitters: [
			Plugin.AliasRedirects(),
			Plugin.Assets(),
			Plugin.ComponentResources(),
			Plugin.ContentIndex(),
			Plugin.ContentPage(),
			Plugin.CustomOgImages(),
			Plugin.Favicon(),
			Plugin.FolderPage(),
			Plugin.TagPage(),
			Plugin.Static(),
			Plugin.NotFoundPage(),
		],
	},
}

export default config