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
}
