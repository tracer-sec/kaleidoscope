#include "graphparser.h"
#include "../lib/sqlite3.h"
#include <QFile>

using namespace std;

GraphParser::GraphParser(Graph *graph) :
    graph_(graph)
{

}

int GraphParser::Load(string filename)
{
    sqlite3 *db;
    sqlite3_open(filename.c_str(), &db);

    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT id, type, name, data, x, y FROM nodes", -1, &statement, nullptr);
    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(statement, 0);
        string type(reinterpret_cast<const string::value_type *>(sqlite3_column_text(statement, 1)));
        string name(reinterpret_cast<const string::value_type *>(sqlite3_column_text(statement, 2)));
        string data(reinterpret_cast<const string::value_type *>(sqlite3_column_text(statement, 3)));
        float x = static_cast<float>(sqlite3_column_double(statement, 4));
        float y = static_cast<float>(sqlite3_column_double(statement, 5));

        Node *n = new Node();
        n->id = id;
        n->type = type;
        n->name = name;
        n->data = data;
        n->position.setX(x);
        n->position.setY(y);

        graph_->AddNode(n);
    }
    sqlite3_finalize(statement);

    sqlite3_prepare_v2(db, "SELECT parent_id, child_id FROM edges", -1, &statement, nullptr);
    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int parentId = sqlite3_column_int(statement, 0);
        int childId = sqlite3_column_int(statement, 1);

        graph_->AddEdge(parentId, childId);
    }
    sqlite3_finalize(statement);

    sqlite3_close(db);

    return 0;
}

int GraphParser::Save(string filename)
{
    QFile::remove(filename.c_str());

    sqlite3 *db;
    sqlite3_open(filename.c_str(), &db);

    string createNodeTable("CREATE TABLE nodes (id INT PRIMARY KEY NOT NULL, type TEXT NOT NULL, name TEXT NOT NULL, data TEXT NOT NULL, x REAL, y REAL)");
    string createEdgeTable("CREATE TABLE edges (parent_id INT NOT NULL, child_id INT NOT NULL)");

    char *error = nullptr;
    sqlite3_exec(db, createNodeTable.c_str(), nullptr, nullptr, &error);
    sqlite3_exec(db, createEdgeTable.c_str(), nullptr, nullptr, &error);

    sqlite3_exec(db, "BEGIN", 0, 0, 0);

    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "INSERT INTO nodes (id, type, name, data, x, y) VALUES (?, ?, ?, ?, ?, ?)", -1, &statement, nullptr);
    for (auto n : graph_->GetNodes())
    {
        sqlite3_bind_int(statement, 1, n->id);
        sqlite3_bind_text(statement, 2, n->type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(statement, 3, n->name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(statement, 4, n->data.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(statement, 5, n->position.x());
        sqlite3_bind_double(statement, 6, n->position.y());
        auto result = sqlite3_step(statement);
        if (result != SQLITE_DONE)
        {
            const char *wtf = sqlite3_errmsg(db);
        }
        sqlite3_reset(statement);
    }
    sqlite3_finalize(statement);

    sqlite3_exec(db, "COMMIT", 0, 0, 0);

    sqlite3_exec(db, "BEGIN", 0, 0, 0);

    sqlite3_stmt *edgeSql;
    sqlite3_prepare_v2(db, "INSERT INTO edges (parent_id, child_id) VALUES (?, ?)", -1, &edgeSql, nullptr);
    for (auto e : graph_->GetEdges())
    {
        sqlite3_bind_int(edgeSql, 1, e.parent->id);
        sqlite3_bind_int(edgeSql, 2, e.child->id);
        auto result = sqlite3_step(edgeSql);
        if (result != SQLITE_DONE)
        {
            const char *wtf = sqlite3_errmsg(db);
        }
        sqlite3_reset(edgeSql);
    }
    sqlite3_finalize(edgeSql);

    sqlite3_exec(db, "COMMIT", 0, 0, 0);

    sqlite3_close(db);

    return 0;
}
