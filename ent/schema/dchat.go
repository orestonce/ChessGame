package schema

import (
	"entgo.io/ent"
	"entgo.io/ent/schema/field"
)

// DChat holds the schema definition for the DChat entity.
type DChat struct {
	ent.Schema
}

// Fields of the DChat.
func (DChat) Fields() []ent.Field {
	return []ent.Field{
		field.String("id"),
		field.String("session_id"),
		field.String("user_id"),
		field.String("room_id"),
		field.String("text"),
		field.Time("create_time"),
	}
}

// Edges of the DChat.
func (DChat) Edges() []ent.Edge {
	return nil
}
