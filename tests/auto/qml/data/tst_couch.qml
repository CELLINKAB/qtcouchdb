import QtTest 1.0
import CouchDB 1.0

TestCase {
    id: testCase
    name: "Couch"

    function test_url_data() {
        return [
            {tag: "empty", baseUrl: "", databaseUrl: "", designDocumentUrl: "", viewUrl: ""},
            {tag: "host", baseUrl: "couch.db/", databaseUrl: "couch.db/tst_db", designDocumentUrl: "couch.db/tst_db/_design/tst_dd", viewUrl: "couch.db/tst_db/_design/tst_dd/_view/tst_view"},
        ]
    }

    function test_url(data) {
        var databaseUrl = Couch.databaseUrl(data.baseUrl, "tst_db")
        var designDocumentUrl = Couch.designDocumentUrl(databaseUrl, "tst_dd")
        var viewUrl = Couch.viewUrl(designDocumentUrl, "tst_view")

        compare(databaseUrl, data.databaseUrl)
        compare(designDocumentUrl, data.designDocumentUrl)
        compare(viewUrl, data.viewUrl)
    }

    function test_query() {
        var query = Couch.query(1, 2, Qt.DescendingOrder, true)
        compare(query.limit, 1)
        compare(query.skip, 2)
        compare(query.order, Qt.DescendingOrder)
        compare(query.includeDocs, true)
    }

    function test_document() {
        var document = Couch.document("foo", "bar", "baz")
        compare(document.id, "foo")
        compare(document.revision, "bar")
        compare(document.content, "baz")
    }

    function test_request_data() {
        return [
            {tag: "databases", method: "listDatabases", url: "couch.db", expectedUrl: "couch.db/_all_dbs"},
            {tag: "design-documents", method: "listDesignDocuments", url: "couch.db/db", expectedUrl: "couch.db/db/_design_docs"},
            {tag: "document-ids", method: "listDocumentIds", url: "couch.db/db", expectedUrl: "couch.db/db/_all_docs"},
            {tag: "full-documents", method: "listFullDocuments", url: "couch.db/db", expectedUrl: "couch.db/db/_all_docs?include_docs=true"},
            {tag: "view-ids", method: "listViewIds", url: "couch.db/db/_design/dd", expectedUrl: "couch.db/db/_design/dd"},
            {tag: "full-views", method: "listFullViews", url: "couch.db/db/_design/dd", expectedUrl: "couch.db/db/_design/dd?include_docs=true"},
            {tag: "row-ids", method: "listRowIds", url: "couch.db/db/_design/dd/_view/v", expectedUrl: "couch.db/db/_design/dd/_view/v"},
            {tag: "full-rows", method: "listFullRows", url: "couch.db/db/_design/dd/_view/v", expectedUrl: "couch.db/db/_design/dd/_view/v?include_docs=true"},
        ]
    }

    function test_request(data) {
        verify(Couch[data.method] !== undefined)
        var request = Couch[data.method](data.url)
        verify(request !== undefined)
        compare(request.url, data.expectedUrl)
    }
}
