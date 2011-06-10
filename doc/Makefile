ASCIIDOC_FLAGS := -a theme=flask -a toc2 -a icons -a iconsdir=icons
DOCS := \
	krawl_by_example.txt \
	typesystem.txt \

DOCS_HTML := $(patsubst %.txt,%.html,$(DOCS))

all: $(DOCS_HTML)
deploy: $(addprefix $(HOME)/storage/web/,$(DOCS_HTML))

krawl.lang: ../extra/krawl.lang
	cp ../extra/krawl.lang .

$(HOME)/storage/web/%.html: %.html
	cp $*.html $(HOME)/storage/web/$*.html

%.html: %.txt krawl.lang
	asciidoc $(ASCIIDOC_FLAGS) $*.txt

clean:
	rm -rf $(DOCS_HTML)
