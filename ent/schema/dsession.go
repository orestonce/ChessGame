package schema

import (
	"entgo.io/ent"
	"entgo.io/ent/schema/field"
)

// DSession holds the schema definition for the DSession entity.
type DSession struct {
	ent.Schema
}

// Fields of the DSession.
func (DSession) Fields() []ent.Field {
	return []ent.Field{
		field.String("id"),
		field.String("user_id").Default(""),
		field.String("room_id").Default(""),
		field.Time("create_time"),
	}
}

// Edges of the DSession.
func (DSession) Edges() []ent.Edge {
	return nil
}
