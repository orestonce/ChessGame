// Code generated by entc, DO NOT EDIT.

package migrate

import (
	"entgo.io/ent/dialect/sql/schema"
	"entgo.io/ent/schema/field"
)

var (
	// DchatsColumns holds the columns for the "dchats" table.
	DchatsColumns = []*schema.Column{
		{Name: "id", Type: field.TypeString},
		{Name: "session_id", Type: field.TypeString},
		{Name: "user_id", Type: field.TypeString},
		{Name: "room_id", Type: field.TypeString},
		{Name: "text", Type: field.TypeString},
		{Name: "create_time", Type: field.TypeTime},
	}
	// DchatsTable holds the schema information for the "dchats" table.
	DchatsTable = &schema.Table{
		Name:       "dchats",
		Columns:    DchatsColumns,
		PrimaryKey: []*schema.Column{DchatsColumns[0]},
	}
	// DroomsColumns holds the columns for the "drooms" table.
	DroomsColumns = []*schema.Column{
		{Name: "id", Type: field.TypeString},
		{Name: "is_game_running", Type: field.TypeBool, Nullable: true, Default: false},
		{Name: "panel", Type: field.TypeString, Nullable: true, Default: ""},
		{Name: "w_user_id", Type: field.TypeString, Nullable: true, Default: ""},
		{Name: "b_user_id", Type: field.TypeString, Nullable: true, Default: ""},
		{Name: "next_turn_user_id", Type: field.TypeString, Nullable: true, Default: ""},
		{Name: "create_time", Type: field.TypeTime},
	}
	// DroomsTable holds the schema information for the "drooms" table.
	DroomsTable = &schema.Table{
		Name:       "drooms",
		Columns:    DroomsColumns,
		PrimaryKey: []*schema.Column{DroomsColumns[0]},
	}
	// DsessionsColumns holds the columns for the "dsessions" table.
	DsessionsColumns = []*schema.Column{
		{Name: "id", Type: field.TypeString},
		{Name: "user_id", Type: field.TypeString, Default: ""},
		{Name: "room_id", Type: field.TypeString, Default: ""},
		{Name: "create_time", Type: field.TypeTime},
	}
	// DsessionsTable holds the schema information for the "dsessions" table.
	DsessionsTable = &schema.Table{
		Name:       "dsessions",
		Columns:    DsessionsColumns,
		PrimaryKey: []*schema.Column{DsessionsColumns[0]},
	}
	// DusersColumns holds the columns for the "dusers" table.
	DusersColumns = []*schema.Column{
		{Name: "id", Type: field.TypeString},
		{Name: "name", Type: field.TypeString, Unique: true},
		{Name: "password_hash", Type: field.TypeString},
		{Name: "create_time", Type: field.TypeTime},
	}
	// DusersTable holds the schema information for the "dusers" table.
	DusersTable = &schema.Table{
		Name:       "dusers",
		Columns:    DusersColumns,
		PrimaryKey: []*schema.Column{DusersColumns[0]},
	}
	// Tables holds all the tables in the schema.
	Tables = []*schema.Table{
		DchatsTable,
		DroomsTable,
		DsessionsTable,
		DusersTable,
	}
)

func init() {
}
