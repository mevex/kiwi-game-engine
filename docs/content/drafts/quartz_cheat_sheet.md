---
title: Quartz Cheat Sheet
description: A collection of all the usefull things to be used when authoring content for quartz.
lang: en
draft: true
---

This page will serve as a repository for resources and references that may be useful when authoring future notes.

# Frontmatter Options

The frontmatter is the YAML block at the top of a note that controls metadata for both Quartz’s site generation and SEO/social previews. It is enclosed between two `---` lines at the beginning of the file.

**title:** Replace the note name as page title and browser tab title.
**description:** Short summary used for SEO and page previews (like shared links previews).
**draft:** `true`/`false` Excludes page from building if true.
**lang:** Specifies the language of the page using ISO 639-1 codes (en, it). Useful for accessibility (screen readers) and SEO.
**aliases:** \[Alias1, Alias2, Alias3\] List of alternative names of the note for backlinks and search.
**tags:** \[Tag1, Tag2, Tag3\] List of tags for grouping and filtering.

# Folder and Tag Listings
Quartz generates index pages for each folder and nested folder. For example, given the following note `content/a/b/note.md`, quartz will generate an index page for `a` and one for `a/b`. This means that in each subfolder we can add an `index.md` note that will be shown in the index page for the folder before its content listings.

Those index pages can be linked to like this: `[[folder/subfolder/]]` with a trailing slash.

Quartz will also create an index page for each unique tag in the vault. To create a custom page to be rendered before the tag listing, the page must be created under `content/tags/` and the note must have the name of the tag. Also, a default global tag index page at `/tags` that displays a list of all the tags is created.

To link to a tag page prefix its name with `tags/`: `[[tags/tagname]]`.

In both cases is recommended to use Obsidian link formatting to change the appearance of the link, e.g. `[[folder/subfolder/ | Subfolder Index]]`.