// Code generated by entc, DO NOT EDIT.

package dchat

const (
	// Label holds the string label denoting the dchat type in the database.
	Label = "dchat"
	// FieldID holds the string denoting the id field in the database.
	FieldID = "id"
	// FieldSessionID holds the string denoting the session_id field in the database.
	FieldSessionID = "session_id"
	// FieldUserID holds the string denoting the user_id field in the database.
	FieldUserID = "user_id"
	// FieldRoomID holds the string denoting the room_id field in the database.
	FieldRoomID = "room_id"
	// FieldText holds the string denoting the text field in the database.
	FieldText = "text"
	// FieldCreateTime holds the string denoting the create_time field in the database.
	FieldCreateTime = "create_time"
	// Table holds the table name of the dchat in the database.
	Table = "dchats"
)

// Columns holds all SQL columns for dchat fields.
var Columns = []string{
	FieldID,
	FieldSessionID,
	FieldUserID,
	FieldRoomID,
	FieldText,
	FieldCreateTime,
}

// ValidColumn reports if the column name is valid (part of the table columns).
func ValidColumn(column string) bool {
	for i := range Columns {
		if column == Columns[i] {
			return true
		}
	}
	return false
}