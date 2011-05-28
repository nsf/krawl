#!/usr/bin/env ruby

$errors = 0

def run_test(f)
	# compile
	%x[crawlc -o test #{f}]
	out = %x[./test]
	gold = IO.read("#{f}.expected") rescue "To be determined"

	if out != gold then
		puts "#{f}: no match!"
		$errors += 1
	end
end

if ARGV.one? then
	%x[crawlc -o test #{ARGV[0]}]
	puts %x[./test]
	File.delete("test")
	exit 0
end

Dir["*.crl"].sort.each do |f|
	run_test(f)
end

if $errors != 0 then
	puts "-------------------------------------------"
	puts "There were #{$errors} unexpected ERRORS!"
end

File.delete("test")
