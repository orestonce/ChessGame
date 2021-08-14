package schema

import (
	"entgo.io/ent"
	"entgo.io/ent/schema/field"
)

// DRoom holds the schema definition for the DRoom entity.
type DRoom struct {
	ent.Schema
}

// Fields of the DRoom.
func (DRoom) Fields() []ent.Field {
	return []ent.Field{
		field.String("id"),
		field.Bool("is_game_running").Optional().Default(false),
		field.String("panel").Optional().Default(""),
		field.String("w_user_id").Optional().Default(""),
		field.String("b_user_id").Optional().Default(""),
		field.String("next_turn_user_id").Optional().Default(""),
		field.Time("create_time"),
	}
}

// Edges of the DRoom.
func (DRoom) Edges() []ent.Edge {
	return nil
}
