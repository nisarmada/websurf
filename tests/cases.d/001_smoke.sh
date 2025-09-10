# Register tests using: add_test "Description" function_name
# Helpers: assert_status, assert_header_contains, assert_body_contains

# 1) Root should return 200
test_root_200() { assert_status GET / 200; }
add_test "GET / returns 200" test_root_200

# 2) Unknown path should be 404 (adjust if your server behaves differently)
test_404() { assert_status GET /no-such-page 404; }
add_test "GET /no-such-page returns 404" test_404

# 3) Content-Type looks like HTML on /
test_content_type_html() { assert_header_contains GET / "Content-Type" "text/html"; }
add_test "Content-Type contains text/html on /" test_content_type_html

# 4) Body contains a marker word (adjust to your index.html)
test_body_contains_marker() { assert_body_contains GET / "html"; }
add_test "Body of / contains 'html'" test_body_contains_marker

# 5) Method not allowed returns 405 (adapt to your rules)
test_method_not_allowed() { assert_status DELETE / 405; }
add_test "DELETE / returns 405 (if disallowed)" test_method_not_allowed
