{
  if (match($0,"變")) {
    do {
      prev=lang;
      getline lang<"po/LINGUAS";
      l=$0;
      gsub("變",lang,l);
      if (prev!=lang) {print l}
    } while (prev!=lang);
  } else if (match($0,"或")) {
    while ((getline def_line<"config.h") > 0) {
      if (def_line == "#define HAVE_LIBWEBP 1") {
        gsub("或","",$0);
        print
        break
      }
    }
  } else {
    print
  }
}
