package tree_sitter_fekal_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_fekal "gitlab.com/emilua/fekal/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_fekal.Language())
	if language == nil {
		t.Errorf("Error loading Fekal grammar")
	}
}
