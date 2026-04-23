#pragma once

#include <userver/formats/json/value.hpp>
#include "../storage/fitness_storage.hpp"

namespace domain::json {

storage::User ParseUser(const userver::formats::json::Value& json);
userver::formats::json::Value SerializeUser(const storage::User& user);

storage::Exercise ParseExercise(const userver::formats::json::Value& json);
userver::formats::json::Value SerializeExercise(const storage::Exercise& ex);

storage::WorkoutExercise ParseWorkoutExercise(const userver::formats::json::Value& json);
userver::formats::json::Value SerializeWorkoutExercise(const storage::WorkoutExercise& ex);

storage::Workout ParseWorkout(const userver::formats::json::Value& json);
userver::formats::json::Value SerializeWorkout(const storage::Workout& w);

userver::formats::json::Value SerializeStats(const storage::Statistics& stats);

}