# REST-like JSON API Routes

The graphical UI remains the primary interface, but UTMS also exposes a small JSON API for testing and future front-end expansion.

## Authentication

### `POST /api/login`

Form fields:

- `username`
- `password`

Returns JSON and sets the `sessionId` cookie on success.

### `POST /api/logout`

Requires a valid session cookie and clears it.

## Read endpoints

### `GET /api/me`

Returns the current authenticated user's profile, courses, and posts.

### `GET /api/users`

Returns all system users with ID, name, and role.

### `GET /api/user?id=<user_id>`

Returns a single user's personal-page data.

### `GET /api/courses`

Returns all course offerings.

### `GET /api/notifications`

Returns and consumes current-user notifications, matching the behavior of the assignment command.

## Security behavior

- API routes require the same session cookie as the HTML dashboard except for `/api/login`.
- Passwords and cookies are redacted in terminal logs.
- User-generated text is JSON-escaped before serialization.
