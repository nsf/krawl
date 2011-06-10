#!/usr/bin/env ruby

$errors = 0
$problems = []

def run_test(f)
	# compile
	%x[krawl -o test #{f}]
	out = %x[./test]
	gold = IO.read("#{f}.expected") rescue "To be determined"

	if out != gold then
		$problems << "#{f}: no match!"
		$errors += 1
		return false
	end
	return true
end

if ARGV.one? then
	%x[krawl -o test #{ARGV[0]}]
	puts %x[./test]
	File.delete("test")
	exit 0
end

Dir["*.krl"].sort.each do |f|
	if run_test(f)
		STDOUT.print("+")
	else
		STDOUT.print("-")
	end
end

puts

if $errors != 0 then
	$problems.each do |p|
		puts p
	end
	puts "-------------------------------------------"
	puts "There were #{$errors} unexpected ERRORS!"
end

File.delete("test")
