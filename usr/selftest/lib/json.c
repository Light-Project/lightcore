/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <json.h>
#include <selftest.h>

static const char json_test[] = {
    "[\n"
    "{\"comment\": \"empty list, empty docs\",\n"
    "\"doc\": {},\n"
    "\"patch\": [],\n"
    "\"expected\": {}},\n"

    "{\"comment\": \"empty patch list\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [],\n"
    "\"expected\": {\"foo\": 1}},\n"

    "{\"comment\": \"rearrangements OK?\",\n"
    "\"doc\": {\"foo\": 1, \"bar\": 2},\n"
    "\"patch\": [],\n"
    "\"expected\": {\"bar\":2, \"foo\": 1}},\n"

    "{\"comment\": \"rearrangements OK?  How about one level down ... array\",\n"
    "\"doc\": [{\"foo\": 1, \"bar\": 2}],\n"
    "\"patch\": [],\n"
    "\"expected\": [{\"bar\":2, \"foo\": 1}]},\n"

    "{\"comment\": \"rearrangements OK?  How about one level down...\",\n"
    "\"doc\": {\"foo\":{\"foo\": 1, \"bar\": 2}},\n"
    "\"patch\": [],\n"
    "\"expected\": {\"foo\":{\"bar\":2, \"foo\": 1}}},\n"

    "{\"comment\": \"add replaces any existing field\",\n"
    "\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/foo\", \"value\":1}],\n"
    "\"expected\": {\"foo\": 1}},\n"

    "{\"comment\": \"toplevel array\",\n"
    "\"doc\": [],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/0\", \"value\": \"foo\"}],\n"
    "\"expected\": [\"foo\"]},\n"

    "{\"comment\": \"toplevel array, no change\",\n"
    "\"doc\": [\"foo\"],\n"
    "\"patch\": [],\n"
    "\"expected\": [\"foo\"]},\n"

    "{\"comment\": \"toplevel object, numeric string\",\n"
    "\"doc\": {},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/foo\", \"value\": \"1\"}],\n"
    "\"expected\": {\"foo\":\"1\"}},\n"

    "{\"comment\": \"toplevel object, integer\",\n"
    "\"doc\": {},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/foo\", \"value\": 1}],\n"
    "\"expected\": {\"foo\":1}},\n"

    "{\"comment\": \"Toplevel scalar values OK?\",\n"
    "\"doc\": \"foo\",\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"\", \"value\": \"bar\"}],\n"
    "\"expected\": \"bar\",\n"
    "\"disabled\": true},\n"

    "{\"comment\": \"replace object document with array document?\",\n"
    "\"doc\": {},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"\", \"value\": []}],\n"
    "\"expected\": []},\n"

    "{\"comment\": \"replace array document with object document?\",\n"
    "\"doc\": [],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"\", \"value\": {}}],\n"
    "\"expected\": {}},\n"

    "{\"comment\": \"append to root array document?\",\n"
    "\"doc\": [],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/-\", \"value\": \"hi\"}],\n"
    "\"expected\": [\"hi\"]},\n"

    "{\"comment\": \"Add, / target\",\n"
    "\"doc\": {},\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/\", \"value\":1 } ],\n"
    "\"expected\": {\"\":1}},\n"

    "{\"comment\": \"Add, /foo/ deep target (trailing slash)\",\n"
    "\"doc\": {\"foo\": {}},\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/foo/\", \"value\":1 } ],\n"
    "\"expected\": {\"foo\":{\"\": 1}}},\n"

    "{\"comment\": \"Add composite value at top level\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar\", \"value\": [1, 2]}],\n"
    "\"expected\": {\"foo\": 1, \"bar\": [1, 2]}},\n"

    "{\"comment\": \"Add into composite value\",\n"
    "\"doc\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/baz/0/foo\", \"value\": \"world\"}],\n"
    "\"expected\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\", \"foo\": \"world\"}]}},\n"

    "{\"doc\": {\"bar\": [1, 2]},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar/8\", \"value\": \"5\"}],\n"
    "\"error\": \"Out of bounds (upper)\"},\n"

    "{\"doc\": {\"bar\": [1, 2]},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar/-1\", \"value\": \"5\"}],\n"
    "\"error\": \"Out of bounds (lower)\"},\n"

    "{\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar\", \"value\": true}],\n"
    "\"expected\": {\"foo\": 1, \"bar\": true}},\n"

    "{\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar\", \"value\": false}],\n"
    "\"expected\": {\"foo\": 1, \"bar\": false}},\n"

    "{\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar\", \"value\": null}],\n"
    "\"expected\": {\"foo\": 1, \"bar\": null}},\n"

    "{\"comment\": \"0 can be an array index or object element name\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/0\", \"value\": \"bar\"}],\n"
    "\"expected\": {\"foo\": 1, \"0\": \"bar\" }},\n"

    "{\"doc\": [\"foo\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/1\", \"value\": \"bar\"}],\n"
    "\"expected\": [\"foo\", \"bar\"]},\n"

    "{\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/1\", \"value\": \"bar\"}],\n"
    "\"expected\": [\"foo\", \"bar\", \"sil\"]},\n"

    "{\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/0\", \"value\": \"bar\"}],\n"
    "\"expected\": [\"bar\", \"foo\", \"sil\"]},\n"

    "{\"comment\": \"push item to array via last index + 1\",\n"
    "\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\":\"add\", \"path\": \"/2\", \"value\": \"bar\"}],\n"
    "\"expected\": [\"foo\", \"sil\", \"bar\"]},\n"

    "{\"comment\": \"add item to array at index > length should fail\",\n"
    "\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\":\"add\", \"path\": \"/3\", \"value\": \"bar\"}],\n"
    "\"error\": \"index is greater than number of items in array\"},\n"

    "{\"comment\": \"test against implementation-specific numeric parsing\",\n"
    "\"doc\": {\"1e0\": \"foo\"},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/1e0\", \"value\": \"foo\"}],\n"
    "\"expected\": {\"1e0\": \"foo\"}},\n"

    "{\"comment\": \"test with bad number should fail\",\n"
    "\"doc\": [\"foo\", \"bar\"],\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/1e0\", \"value\": \"bar\"}],\n"
    "\"error\": \"test op shouldn't get array element 1\"},\n"

    "{\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/bar\", \"value\": 42}],\n"
    "\"error\": \"Object operation on array target\"},\n"

    "{\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/1\", \"value\": [\"bar\", \"baz\"]}],\n"
    "\"expected\": [\"foo\", [\"bar\", \"baz\"], \"sil\"],\n"
    "\"comment\": \"value in array add not flattened\"},\n"

    "{\"doc\": {\"foo\": 1, \"bar\": [1, 2, 3, 4]},\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/bar\"}],\n"
    "\"expected\": {\"foo\": 1}},\n"

    "{\"doc\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/baz/0/qux\"}],\n"
    "\"expected\": {\"foo\": 1, \"baz\": [{}]}},\n"

    "{\"doc\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/foo\", \"value\": [1, 2, 3, 4]}],\n"
    "\"expected\": {\"foo\": [1, 2, 3, 4], \"baz\": [{\"qux\": \"hello\"}]}},\n"

    "{\"doc\": {\"foo\": [1, 2, 3, 4], \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/baz/0/qux\", \"value\": \"world\"}],\n"
    "\"expected\": {\"foo\": [1, 2, 3, 4], \"baz\": [{\"qux\": \"world\"}]}},\n"

    "{\"doc\": [\"foo\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/0\", \"value\": \"bar\"}],\n"
    "\"expected\": [\"bar\"]},\n"

    "{\"doc\": [\"\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/0\", \"value\": 0}],\n"
    "\"expected\": [0]},\n"

    "{\"doc\": [\"\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/0\", \"value\": true}],\n"
    "\"expected\": [true]},\n"

    "{\"doc\": [\"\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/0\", \"value\": false}],\n"
    "\"expected\": [false]},\n"

    "{\"doc\": [\"\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/0\", \"value\": null}],\n"
    "\"expected\": [null]},\n"

    "{\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/1\", \"value\": [\"bar\", \"baz\"]}],\n"
    "\"expected\": [\"foo\", [\"bar\", \"baz\"]],\n"
    "\"comment\": \"value in array replace not flattened\"},\n"

    "{\"comment\": \"replace whole document\",\n"
    "\"doc\": {\"foo\": \"bar\"},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"\", \"value\": {\"baz\": \"qux\"}}],\n"
    "\"expected\": {\"baz\": \"qux\"}},\n"

    "{\"comment\": \"test replace with missing parent key should fail\",\n"
    "\"doc\": {\"bar\": \"baz\"},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/foo/bar\", \"value\": false}],\n"
    "\"error\": \"replace op should fail with missing parent key\"},\n"

    "{\"comment\": \"spurious patch properties\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": 1, \"spurious\": 1}],\n"
    "\"expected\": {\"foo\": 1}},\n"

    "{\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": null}],\n"
    "\"expected\": {\"foo\": null},\n"
    "\"comment\": \"null value should be valid obj property\"},\n"

    "{\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/foo\", \"value\": \"truthy\"}],\n"
    "\"expected\": {\"foo\": \"truthy\"},\n"
    "\"comment\": \"null value should be valid obj property to be replaced with something truthy\"},\n"

    "{\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"move\", \"from\": \"/foo\", \"path\": \"/bar\"}],\n"
    "\"expected\": {\"bar\": null},\n"
    "\"comment\": \"null value should be valid obj property to be moved\"},\n"

    "{\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"copy\", \"from\": \"/foo\", \"path\": \"/bar\"}],\n"
    "\"expected\": {\"foo\": null, \"bar\": null},\n"
    "\"comment\": \"null value should be valid obj property to be copied\"},\n"

    "{\"doc\": {\"foo\": null},\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/foo\"}],\n"
    "\"expected\": {},\n"
    "\"comment\": \"null value should be valid obj property to be removed\"},\n"

    "{\"doc\": {\"foo\": \"bar\"},\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/foo\", \"value\": null}],\n"
    "\"expected\": {\"foo\": null},\n"
    "\"comment\": \"null value should still be valid obj property replace other value\"},\n"

    "{\"doc\": {\"foo\": {\"foo\": 1, \"bar\": 2}},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": {\"bar\": 2, \"foo\": 1}}],\n"
    "\"expected\": {\"foo\": {\"foo\": 1, \"bar\": 2}},\n"
    "\"comment\": \"test should pass despite rearrangement\"},\n"

    "{\"doc\": {\"foo\": [{\"foo\": 1, \"bar\": 2}]},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": [{\"bar\": 2, \"foo\": 1}]}],\n"
    "\"expected\": {\"foo\": [{\"foo\": 1, \"bar\": 2}]},\n"
    "\"comment\": \"test should pass despite (nested) rearrangement\"},\n"

    "{\"doc\": {\"foo\": {\"bar\": [1, 2, 5, 4]}},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": {\"bar\": [1, 2, 5, 4]}}],\n"
    "\"expected\": {\"foo\": {\"bar\": [1, 2, 5, 4]}},\n"
    "\"comment\": \"test should pass - no error\"},\n"

    "{\"doc\": {\"foo\": {\"bar\": [1, 2, 5, 4]}},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": [1, 2]}],\n"
    "\"error\": \"test op should fail\"},\n"

    "{\"comment\": \"Whole document\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"\", \"value\": {\"foo\": 1}}],\n"
    "\"disabled\": true},\n"

    "{\"comment\": \"Empty-string element\",\n"
    "\"doc\": {\"\": 1},\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/\", \"value\": 1}],\n"
    "\"expected\": {\"\": 1 }},\n"

    "{\"doc\": {\n"
    "        \"foo\": [\"bar\", \"baz\"],\n"
    "        \"\": 0,\n"
    "        \"a/b\": 1,\n"
    "        \"c%d\": 2,\n"
    "        \"e^f\": 3,\n"
    "        \"g|h\": 4,\n"
    "        \"i\\j\": 5,\n"
    "        \"k\\\"l\": 6,\n"
    "        \" \": 7,\n"
    "        \"m~n\": 8\n"
    "    },\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/foo\", \"value\": [\"bar\", \"baz\"]},\n"
    "            {\"op\": \"test\", \"path\": \"/foo/0\", \"value\": \"bar\"},\n"
    "            {\"op\": \"test\", \"path\": \"/\", \"value\": 0},\n"
    "            {\"op\": \"test\", \"path\": \"/a~1b\", \"value\": 1},\n"
    "            {\"op\": \"test\", \"path\": \"/c%d\", \"value\": 2},\n"
    "            {\"op\": \"test\", \"path\": \"/e^f\", \"value\": 3},\n"
    "            {\"op\": \"test\", \"path\": \"/g|h\", \"value\": 4},\n"
    "            {\"op\": \"test\", \"path\":  \"/i\\j\", \"value\": 5},\n"
    "            {\"op\": \"test\", \"path\": \"/k\\\"l\", \"value\": 6},\n"
    "            {\"op\": \"test\", \"path\": \"/ \", \"value\": 7},\n"
    "            {\"op\": \"test\", \"path\": \"/m~0n\", \"value\": 8}],\n"
    "\"expected\": {\n"
    "        \"\": 0,\n"
    "        \" \": 7,\n"
    "        \"a/b\": 1,\n"
    "        \"c%d\": 2,\n"
    "        \"e^f\": 3,\n"
    "        \"foo\": [\n"
    "            \"bar\",\n"
    "            \"baz\"\n"
    "        ],\n"
    "        \"g|h\": 4,\n"
    "        \"i\\j\": 5,\n"
    "        \"k\\\"l\": 6,\n"
    "        \"m~n\": 8\n"
    "    }\n"

    "{\"comment\": \"Move to same location has no effect\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"move\", \"from\": \"/foo\", \"path\": \"/foo\"}],\n"
    "\"expected\": {\"foo\": 1}},\n"

    "{\"doc\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"move\", \"from\": \"/foo\", \"path\": \"/bar\"}],\n"
    "\"expected\": {\"baz\": [{\"qux\": \"hello\"}], \"bar\": 1}},\n"

    "{\"doc\": {\"baz\": [{\"qux\": \"hello\"}], \"bar\": 1},\n"
    "\"patch\": [{\"op\": \"move\", \"from\": \"/baz/0/qux\", \"path\": \"/baz/1\"}],\n"
    "\"expected\": {\"baz\": [{}, \"hello\"], \"bar\": 1}},\n"

    "{\"doc\": {\"baz\": [{\"qux\": \"hello\"}], \"bar\": 1},\n"
    "\"patch\": [{\"op\": \"copy\", \"from\": \"/baz/0\", \"path\": \"/boo\"}],\n"
    "\"expected\": {\"baz\":[{\"qux\":\"hello\"}],\"bar\":1,\"boo\":{\"qux\":\"hello\"}}},\n"

    "{\"comment\": \"replacing the root of the document is possible with add\",\n"
    "\"doc\": {\"foo\": \"bar\"},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"\", \"value\": {\"baz\": \"qux\"}}],\n"
    "\"expected\": {\"baz\":\"qux\"}},\n"

    "{\"comment\": \"Adding to \\\"/-\\\" adds to the end of the array\",\n"
    "\"doc\": [ 1, 2 ],\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/-\", \"value\": {\"foo\": [ \"bar\", \"baz\" ] } } ],\n"
    "\"expected\": [ 1, 2, {\"foo\": [ \"bar\", \"baz\" ] } ]},\n"

    "{\"comment\": \"Adding to \\\"/-\\\" adds to the end of the array, even n levels down\",\n"
    "\"doc\": [ 1, 2, [ 3, [ 4, 5 ] ] ],\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/2/1/-\", \"value\": {\"foo\": [ \"bar\", \"baz\" ] } } ],\n"
    "\"expected\": [ 1, 2, [ 3, [ 4, 5, {\"foo\": [ \"bar\", \"baz\" ] } ] ] ]},\n"

    "{\"comment\": \"test remove with bad number should fail\",\n"
    "\"doc\": {\"foo\": 1, \"baz\": [{\"qux\": \"hello\"}]},\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/baz/1e0/qux\"}],\n"
    "\"error\": \"remove op shouldn't remove from array with bad number\"},\n"

    "{\"comment\": \"test remove on array\",\n"
    "\"doc\": [1, 2, 3, 4],\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/0\"}],\n"
    "\"expected\": [2, 3, 4]},\n"

    "{\"comment\": \"test repeated removes\",\n"
    "\"doc\": [1, 2, 3, 4],\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/1\"},\n"
    "            {\"op\": \"remove\", \"path\": \"/2\" }],\n"
    "\"expected\": [1, 3]},\n"

    "{\"comment\": \"test remove with bad index should fail\",\n"
    "\"doc\": [1, 2, 3, 4],\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/1e0\"}],\n"
    "\"error\": \"remove op shouldn't remove from array with bad number\"},\n"

    "{\"comment\": \"test replace with bad number should fail\",\n"
    "\"doc\": [\"\"],\n"
    "\"patch\": [{\"op\": \"replace\", \"path\": \"/1e0\", \"value\": false}],\n"
    "\"error\": \"replace op shouldn't replace in array with bad number\"},\n"

    "{\"comment\": \"test copy with bad number should fail\",\n"
    "\"doc\": {\"baz\": [1,2,3], \"bar\": 1},\n"
    "\"patch\": [{\"op\": \"copy\", \"from\": \"/baz/1e0\", \"path\": \"/boo\"}],\n"
    "\"error\": \"copy op shouldn't work with bad number\"},\n"

    "{\"comment\": \"test move with bad number should fail\",\n"
    "\"doc\": {\"foo\": 1, \"baz\": [1,2,3,4]},\n"
    "\"patch\": [{\"op\": \"move\", \"from\": \"/baz/1e0\", \"path\": \"/foo\"}],\n"
    "\"error\": \"move op shouldn't work with bad number\"},\n"

    "{\"comment\": \"test add with bad number should fail\",\n"
    "\"doc\": [\"foo\", \"sil\"],\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/1e0\", \"value\": \"bar\"}],\n"
    "\"error\": \"add op shouldn't add to array with bad number\"},\n"

    "{\"comment\": \"missing 'value' parameter to add\",\n"
    "\"doc\": [ 1 ],\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/-\" } ],\n"
    "\"error\": \"missing 'value' parameter\"},\n"

    "{\"comment\": \"missing 'value' parameter to replace\",\n"
    "\"doc\": [ 1 ],\n"
    "\"patch\": [ {\"op\": \"replace\", \"path\": \"/0\" } ],\n"
    "\"error\": \"missing 'value' parameter\"},\n"

    "{\"comment\": \"missing 'value' parameter to test\",\n"
    "\"doc\": [ null ],\n"
    "\"patch\": [ {\"op\": \"test\", \"path\": \"/0\" } ],\n"
    "\"error\": \"missing 'value' parameter\"},\n"

    "{\"comment\": \"missing value parameter to test - where undef is falsy\",\n"
    "\"doc\": [ false ],\n"
    "\"patch\": [ {\"op\": \"test\", \"path\": \"/0\" } ],\n"
    "\"error\": \"missing 'value' parameter\"},\n"

    "{\"comment\": \"missing from parameter to copy\",\n"
    "\"doc\": [ 1 ],\n"
    "\"patch\": [ {\"op\": \"copy\", \"path\": \"/-\" } ],\n"
    "\"error\": \"missing 'from' parameter\"},\n"

    "{\"comment\": \"missing from location to copy\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [ {\"op\": \"copy\", \"from\": \"/bar\", \"path\": \"/foo\" } ],\n"
    "\"error\": \"missing 'from' location\"},\n"

    "{\"comment\": \"missing from parameter to move\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [ {\"op\": \"move\", \"path\": \"\" } ],\n"
    "\"error\": \"missing 'from' parameter\"},\n"

    "{\"comment\": \"missing from location to move\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [ {\"op\": \"move\", \"from\": \"/bar\", \"path\": \"/foo\" } ],\n"
    "\"error\": \"missing 'from' location\"},\n"

    "{\"comment\": \"duplicate ops\",\n"
    "\"doc\": {\"foo\": \"bar\"},\n"
    "\"patch\": [ {\"op\": \"add\", \"path\": \"/baz\", \"value\": \"qux\",\n"
    "            \"op\": \"move\", \"from\":\"/foo\" } ],\n"
    "\"error\": \"patch has two 'op' members\",\n"
    "\"disabled\": true},\n"

    "{\"comment\": \"unrecognized op should fail\",\n"
    "\"doc\": {\"foo\": 1},\n"
    "\"patch\": [{\"op\": \"spam\", \"path\": \"/foo\", \"value\": 1}],\n"
    "\"error\": \"Unrecognized op 'spam'\"},\n"

    "{\"comment\": \"test with bad array number that has leading zeros\",\n"
    "\"doc\": [\"foo\", \"bar\"],\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/00\", \"value\": \"foo\"}],\n"
    "\"error\": \"test op should reject the array value, it has leading zeros\"},\n"

    "{\"comment\": \"test with bad array number that has leading zeros\",\n"
    "\"doc\": [\"foo\", \"bar\"],\n"
    "\"patch\": [{\"op\": \"test\", \"path\": \"/01\", \"value\": \"bar\"}],\n"
    "\"error\": \"test op should reject the array value, it has leading zeros\"},\n"

    "{\"comment\": \"Removing nonexistent field\",\n"
    "\"doc\": {\"foo\" : \"bar\"},\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/baz\"}],\n"
    "\"error\": \"removing a nonexistent field should fail\"},\n"

    "{\"comment\": \"Removing nonexistent index\",\n"
    "\"doc\": [\"foo\", \"bar\"],\n"
    "\"patch\": [{\"op\": \"remove\", \"path\": \"/2\"}],\n"
    "\"error\": \"removing a nonexistent index should fail\"},\n"

    "{\"comment\": \"Patch with different capitalisation than doc\",\n"
    "\"doc\": {\"foo\":\"bar\"},\n"
    "\"patch\": [{\"op\": \"add\", \"path\": \"/FOO\", \"value\": \"BAR\"}],\n"
    "\"expected\": {\"foo\": \"bar\", \"FOO\": \"BAR\"}}\n"
    "]\n"
};

static void json_dumpinfo(struct kshell_context *ctx, struct json_node *parent, unsigned int depth)
{
    struct json_node *child;
    unsigned int count;

    if (json_test_array(parent))
        kshell_printf(ctx, "array: %s {\n", parent->name);
    else if (json_test_object(parent))
        kshell_printf(ctx, "object: %s {\n", parent->name);

    list_for_each_entry(child, &parent->child, sibling) {
        for (count = 0; count < depth; ++count)
            kshell_puts(ctx, "    ");
        if (json_test_array(child) || json_test_object(child)) {
            json_dumpinfo(ctx, child, depth + 1);
            continue;
        }
        kshell_printf(ctx, "%s: ", child->name);
        if (json_test_number(child))
            kshell_printf(ctx, "(%ld)", child->number);
        else if (json_test_string(child))
            kshell_printf(ctx, "'%s'", child->string);
        else if (json_test_null(child))
            kshell_puts(ctx, "null");
        else if (json_test_true(child))
            kshell_puts(ctx, "true");
        else /* json_test_false(child) */
            kshell_puts(ctx, "false");
        kshell_putc(ctx, '\n');
    }

    for (count = 0; count < depth - 1; ++count)
        kshell_puts(ctx, "    ");
    kshell_puts(ctx, "}\n");
}

static state json_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct json_node *jnode;
    state retval;
    char *buff;
    int length;

    retval = json_parse(json_test, &jnode, GFP_KERNEL);
    if (retval)
        return retval;

    kshell_puts(ctx, "pseudo expression:\n");
    json_dumpinfo(ctx, jnode, 1);

    kshell_puts(ctx, "json encode:\n");
    length = json_encode(jnode, NULL, 0);

    buff = kmalloc(length, GFP_KERNEL);
    if (!buff) {
        retval = -ENOMEM;
        goto finish;
    }

    length = json_encode(jnode, buff, length);
    kshell_write(ctx, buff, length);
    kfree(buff);

finish:
    json_release(jnode);
    return retval;
}

static struct selftest_command json_test_command = {
    .group = "lib",
    .name = "json",
    .desc = "json unit test",
    .testing = json_test_testing,
};

static state selftest_json_init(void)
{
    return selftest_register(&json_test_command);
}
kshell_initcall(selftest_json_init);
