#ifndef UTILITIES_HXX
#define UTILITIES_HXX

//_____________________________________________________________________________
void ParseTreeFilename(TString name, TString &dirname, TString &filename, TString &listname, TString &treename) {
  //
  // Get information from the name
  // - input: name
  // - outputs: dirname, filename, listname, treename
  //

  dirname.Clear();
  filename.Clear();
  listname.Clear();
  treename.Clear();

  // find where .root is
  TString dot_root = ".root";
  Int_t js = name.Index(dot_root);

  // split name in two:
  // first half: before & including .root, to extract dirname and filename
  TString first_half = name(0, js + dot_root.Length());
  dirname = first_half(0, first_half.Last('/'));
  filename = first_half(first_half.Last('/') + 1, first_half.Length());

  // second half: after .root, to extract listname and treename
  TString second_half = name(js + dot_root.Length(), name.Length() - js);
  listname = second_half(1, second_half.Last('/') - 1);
  treename = second_half(second_half.Last('/') + 1, second_half.Length());

  // debug
  /*
  std::cout << "ParseTreeFilename :: dirname  = " << dirname << std::endl;
  std::cout << "ParseTreeFilename :: filename = " << filename << std::endl;
  std::cout << "ParseTreeFilename :: listname = " << listname << std::endl;
  std::cout << "ParseTreeFilename :: treename = " << treename << std::endl;
  */
}

//_____________________________________________________________________________
void AddFileToList(TList *list, const char *name) {
  //
  // Add a new file to this list
  //

  TString dirname, basename, listname, treename;
  ParseTreeFilename(name, dirname, basename, listname, treename);

  TFile *input_file = new TFile(dirname + "/" + basename, "READ");
  TList *input_list = (TList *)input_file->Get(listname);
  TTree *input_tree = (TTree *)input_list->FindObject(treename);

  list->Add(input_tree);
}

//_____________________________________________________________________________
void AddTreesToList(TList *list, const char *name) {
  //
  //
  //

  TString directory, basename, listname, treename;
  ParseTreeFilename(name, directory, basename, listname, treename);

  // case with one single file
  if (!basename.MaybeWildcard()) {
    return AddFileToList(list, name);
  }  // closure

  // wildcarding used in name
  // printf("AddTreesToList :: directory = %s\n", directory.Data());

  const char *epath = gSystem->ExpandPathName(directory.Data());
  void *dir = gSystem->OpenDirectory(epath);
  delete[] epath;

  const char *file;

  if (dir) {
    // create a TList to store the filenames (not yet sorted)
    TList l;
    TRegexp re(basename, kTRUE);
    while ((file = gSystem->GetDirEntry(dir))) {
      if (!strcmp(file, ".") || !strcmp(file, "..")) {
        continue;
      }
      TString s = file;
      if ((basename != file) && s.Index(re) == kNPOS) {
        continue;
      }
      // printf("AddTreesToList :: file = %s\n", file);
      l.Add(new TObjString(file));
    }
    gSystem->FreeDirectory(dir);

    // sort the files in alphanumeric order
    l.Sort();

    TIter next(&l);
    TObjString *obj;
    while ((obj = (TObjString *)next())) {
      file = obj->GetName();
      AddFileToList(list, TString::Format("%s/%s/%s/%s", directory.Data(), file, listname.Data(), treename.Data()));
    }
    l.Delete();
  }
}

#endif
