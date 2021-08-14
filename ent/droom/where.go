// Code generated by entc, DO NOT EDIT.

package droom

import (
	"time"

	"entgo.io/ent/dialect/sql"
	"github.com/orestonce/ChessGame/ent/predicate"
)

// ID filters vertices based on their ID field.
func ID(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldID), id))
	})
}

// IDEQ applies the EQ predicate on the ID field.
func IDEQ(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldID), id))
	})
}

// IDNEQ applies the NEQ predicate on the ID field.
func IDNEQ(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldID), id))
	})
}

// IDIn applies the In predicate on the ID field.
func IDIn(ids ...string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(ids) == 0 {
			s.Where(sql.False())
			return
		}
		v := make([]interface{}, len(ids))
		for i := range v {
			v[i] = ids[i]
		}
		s.Where(sql.In(s.C(FieldID), v...))
	})
}

// IDNotIn applies the NotIn predicate on the ID field.
func IDNotIn(ids ...string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(ids) == 0 {
			s.Where(sql.False())
			return
		}
		v := make([]interface{}, len(ids))
		for i := range v {
			v[i] = ids[i]
		}
		s.Where(sql.NotIn(s.C(FieldID), v...))
	})
}

// IDGT applies the GT predicate on the ID field.
func IDGT(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GT(s.C(FieldID), id))
	})
}

// IDGTE applies the GTE predicate on the ID field.
func IDGTE(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GTE(s.C(FieldID), id))
	})
}

// IDLT applies the LT predicate on the ID field.
func IDLT(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LT(s.C(FieldID), id))
	})
}

// IDLTE applies the LTE predicate on the ID field.
func IDLTE(id string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LTE(s.C(FieldID), id))
	})
}

// IsGameRunning applies equality check predicate on the "is_game_running" field. It's identical to IsGameRunningEQ.
func IsGameRunning(v bool) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldIsGameRunning), v))
	})
}

// Panel applies equality check predicate on the "panel" field. It's identical to PanelEQ.
func Panel(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldPanel), v))
	})
}

// UpUserID applies equality check predicate on the "up_user_id" field. It's identical to UpUserIDEQ.
func UpUserID(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldUpUserID), v))
	})
}

// DownUserID applies equality check predicate on the "down_user_id" field. It's identical to DownUserIDEQ.
func DownUserID(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldDownUserID), v))
	})
}

// CreateTime applies equality check predicate on the "create_time" field. It's identical to CreateTimeEQ.
func CreateTime(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldCreateTime), v))
	})
}

// IsGameRunningEQ applies the EQ predicate on the "is_game_running" field.
func IsGameRunningEQ(v bool) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldIsGameRunning), v))
	})
}

// IsGameRunningNEQ applies the NEQ predicate on the "is_game_running" field.
func IsGameRunningNEQ(v bool) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldIsGameRunning), v))
	})
}

// IsGameRunningIsNil applies the IsNil predicate on the "is_game_running" field.
func IsGameRunningIsNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.IsNull(s.C(FieldIsGameRunning)))
	})
}

// IsGameRunningNotNil applies the NotNil predicate on the "is_game_running" field.
func IsGameRunningNotNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NotNull(s.C(FieldIsGameRunning)))
	})
}

// PanelEQ applies the EQ predicate on the "panel" field.
func PanelEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldPanel), v))
	})
}

// PanelNEQ applies the NEQ predicate on the "panel" field.
func PanelNEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldPanel), v))
	})
}

// PanelIn applies the In predicate on the "panel" field.
func PanelIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.In(s.C(FieldPanel), v...))
	})
}

// PanelNotIn applies the NotIn predicate on the "panel" field.
func PanelNotIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.NotIn(s.C(FieldPanel), v...))
	})
}

// PanelGT applies the GT predicate on the "panel" field.
func PanelGT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GT(s.C(FieldPanel), v))
	})
}

// PanelGTE applies the GTE predicate on the "panel" field.
func PanelGTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GTE(s.C(FieldPanel), v))
	})
}

// PanelLT applies the LT predicate on the "panel" field.
func PanelLT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LT(s.C(FieldPanel), v))
	})
}

// PanelLTE applies the LTE predicate on the "panel" field.
func PanelLTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LTE(s.C(FieldPanel), v))
	})
}

// PanelContains applies the Contains predicate on the "panel" field.
func PanelContains(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.Contains(s.C(FieldPanel), v))
	})
}

// PanelHasPrefix applies the HasPrefix predicate on the "panel" field.
func PanelHasPrefix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasPrefix(s.C(FieldPanel), v))
	})
}

// PanelHasSuffix applies the HasSuffix predicate on the "panel" field.
func PanelHasSuffix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasSuffix(s.C(FieldPanel), v))
	})
}

// PanelIsNil applies the IsNil predicate on the "panel" field.
func PanelIsNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.IsNull(s.C(FieldPanel)))
	})
}

// PanelNotNil applies the NotNil predicate on the "panel" field.
func PanelNotNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NotNull(s.C(FieldPanel)))
	})
}

// PanelEqualFold applies the EqualFold predicate on the "panel" field.
func PanelEqualFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EqualFold(s.C(FieldPanel), v))
	})
}

// PanelContainsFold applies the ContainsFold predicate on the "panel" field.
func PanelContainsFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.ContainsFold(s.C(FieldPanel), v))
	})
}

// UpUserIDEQ applies the EQ predicate on the "up_user_id" field.
func UpUserIDEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldUpUserID), v))
	})
}

// UpUserIDNEQ applies the NEQ predicate on the "up_user_id" field.
func UpUserIDNEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldUpUserID), v))
	})
}

// UpUserIDIn applies the In predicate on the "up_user_id" field.
func UpUserIDIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.In(s.C(FieldUpUserID), v...))
	})
}

// UpUserIDNotIn applies the NotIn predicate on the "up_user_id" field.
func UpUserIDNotIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.NotIn(s.C(FieldUpUserID), v...))
	})
}

// UpUserIDGT applies the GT predicate on the "up_user_id" field.
func UpUserIDGT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GT(s.C(FieldUpUserID), v))
	})
}

// UpUserIDGTE applies the GTE predicate on the "up_user_id" field.
func UpUserIDGTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GTE(s.C(FieldUpUserID), v))
	})
}

// UpUserIDLT applies the LT predicate on the "up_user_id" field.
func UpUserIDLT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LT(s.C(FieldUpUserID), v))
	})
}

// UpUserIDLTE applies the LTE predicate on the "up_user_id" field.
func UpUserIDLTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LTE(s.C(FieldUpUserID), v))
	})
}

// UpUserIDContains applies the Contains predicate on the "up_user_id" field.
func UpUserIDContains(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.Contains(s.C(FieldUpUserID), v))
	})
}

// UpUserIDHasPrefix applies the HasPrefix predicate on the "up_user_id" field.
func UpUserIDHasPrefix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasPrefix(s.C(FieldUpUserID), v))
	})
}

// UpUserIDHasSuffix applies the HasSuffix predicate on the "up_user_id" field.
func UpUserIDHasSuffix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasSuffix(s.C(FieldUpUserID), v))
	})
}

// UpUserIDIsNil applies the IsNil predicate on the "up_user_id" field.
func UpUserIDIsNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.IsNull(s.C(FieldUpUserID)))
	})
}

// UpUserIDNotNil applies the NotNil predicate on the "up_user_id" field.
func UpUserIDNotNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NotNull(s.C(FieldUpUserID)))
	})
}

// UpUserIDEqualFold applies the EqualFold predicate on the "up_user_id" field.
func UpUserIDEqualFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EqualFold(s.C(FieldUpUserID), v))
	})
}

// UpUserIDContainsFold applies the ContainsFold predicate on the "up_user_id" field.
func UpUserIDContainsFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.ContainsFold(s.C(FieldUpUserID), v))
	})
}

// DownUserIDEQ applies the EQ predicate on the "down_user_id" field.
func DownUserIDEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldDownUserID), v))
	})
}

// DownUserIDNEQ applies the NEQ predicate on the "down_user_id" field.
func DownUserIDNEQ(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldDownUserID), v))
	})
}

// DownUserIDIn applies the In predicate on the "down_user_id" field.
func DownUserIDIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.In(s.C(FieldDownUserID), v...))
	})
}

// DownUserIDNotIn applies the NotIn predicate on the "down_user_id" field.
func DownUserIDNotIn(vs ...string) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.NotIn(s.C(FieldDownUserID), v...))
	})
}

// DownUserIDGT applies the GT predicate on the "down_user_id" field.
func DownUserIDGT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GT(s.C(FieldDownUserID), v))
	})
}

// DownUserIDGTE applies the GTE predicate on the "down_user_id" field.
func DownUserIDGTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GTE(s.C(FieldDownUserID), v))
	})
}

// DownUserIDLT applies the LT predicate on the "down_user_id" field.
func DownUserIDLT(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LT(s.C(FieldDownUserID), v))
	})
}

// DownUserIDLTE applies the LTE predicate on the "down_user_id" field.
func DownUserIDLTE(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LTE(s.C(FieldDownUserID), v))
	})
}

// DownUserIDContains applies the Contains predicate on the "down_user_id" field.
func DownUserIDContains(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.Contains(s.C(FieldDownUserID), v))
	})
}

// DownUserIDHasPrefix applies the HasPrefix predicate on the "down_user_id" field.
func DownUserIDHasPrefix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasPrefix(s.C(FieldDownUserID), v))
	})
}

// DownUserIDHasSuffix applies the HasSuffix predicate on the "down_user_id" field.
func DownUserIDHasSuffix(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.HasSuffix(s.C(FieldDownUserID), v))
	})
}

// DownUserIDIsNil applies the IsNil predicate on the "down_user_id" field.
func DownUserIDIsNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.IsNull(s.C(FieldDownUserID)))
	})
}

// DownUserIDNotNil applies the NotNil predicate on the "down_user_id" field.
func DownUserIDNotNil() predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NotNull(s.C(FieldDownUserID)))
	})
}

// DownUserIDEqualFold applies the EqualFold predicate on the "down_user_id" field.
func DownUserIDEqualFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EqualFold(s.C(FieldDownUserID), v))
	})
}

// DownUserIDContainsFold applies the ContainsFold predicate on the "down_user_id" field.
func DownUserIDContainsFold(v string) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.ContainsFold(s.C(FieldDownUserID), v))
	})
}

// CreateTimeEQ applies the EQ predicate on the "create_time" field.
func CreateTimeEQ(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.EQ(s.C(FieldCreateTime), v))
	})
}

// CreateTimeNEQ applies the NEQ predicate on the "create_time" field.
func CreateTimeNEQ(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.NEQ(s.C(FieldCreateTime), v))
	})
}

// CreateTimeIn applies the In predicate on the "create_time" field.
func CreateTimeIn(vs ...time.Time) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.In(s.C(FieldCreateTime), v...))
	})
}

// CreateTimeNotIn applies the NotIn predicate on the "create_time" field.
func CreateTimeNotIn(vs ...time.Time) predicate.DRoom {
	v := make([]interface{}, len(vs))
	for i := range v {
		v[i] = vs[i]
	}
	return predicate.DRoom(func(s *sql.Selector) {
		// if not arguments were provided, append the FALSE constants,
		// since we can't apply "IN ()". This will make this predicate falsy.
		if len(v) == 0 {
			s.Where(sql.False())
			return
		}
		s.Where(sql.NotIn(s.C(FieldCreateTime), v...))
	})
}

// CreateTimeGT applies the GT predicate on the "create_time" field.
func CreateTimeGT(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GT(s.C(FieldCreateTime), v))
	})
}

// CreateTimeGTE applies the GTE predicate on the "create_time" field.
func CreateTimeGTE(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.GTE(s.C(FieldCreateTime), v))
	})
}

// CreateTimeLT applies the LT predicate on the "create_time" field.
func CreateTimeLT(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LT(s.C(FieldCreateTime), v))
	})
}

// CreateTimeLTE applies the LTE predicate on the "create_time" field.
func CreateTimeLTE(v time.Time) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s.Where(sql.LTE(s.C(FieldCreateTime), v))
	})
}

// And groups predicates with the AND operator between them.
func And(predicates ...predicate.DRoom) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s1 := s.Clone().SetP(nil)
		for _, p := range predicates {
			p(s1)
		}
		s.Where(s1.P())
	})
}

// Or groups predicates with the OR operator between them.
func Or(predicates ...predicate.DRoom) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		s1 := s.Clone().SetP(nil)
		for i, p := range predicates {
			if i > 0 {
				s1.Or()
			}
			p(s1)
		}
		s.Where(s1.P())
	})
}

// Not applies the not operator on the given predicate.
func Not(p predicate.DRoom) predicate.DRoom {
	return predicate.DRoom(func(s *sql.Selector) {
		p(s.Not())
	})
}
