#!/usr/bin/env ruby

tokens = IO.read("tokens.hpp") rescue nil
puts '#include "tokens.hpp"

const char *token_to_string(int tok)
{
	switch (tok) {'

tokens.each_line do |l|
	l.sub(/TOK_[A-Z_]+/) do |tok|
		puts "\tcase #{tok}: return \"#{tok}\";"
	end
end

puts '	}
	return "???";
}'
