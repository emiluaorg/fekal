import XCTest
import SwiftTreeSitter
import TreeSitterFekal

final class TreeSitterFekalTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_fekal())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Fekal grammar")
    }
}
