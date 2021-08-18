package schema

import (
	"entgo.io/ent"
	"entgo.io/ent/schema/field"
)

// DChessdbCache holds the schema definition for the DChessdbCache entity.
type DChessdbCache struct {
	ent.Schema
}

// Fields of the DChessdbCache.
func (DChessdbCache) Fields() []ent.Field {
	return []ent.Field{
		field.String("id"),
		field.String("board").Unique(),
		field.Bytes("resp"),
		field.Time("create_time"),
	}
}

// Edges of the DChessdbCache.
func (DChessdbCache) Edges() []ent.Edge {
	return nil
}
