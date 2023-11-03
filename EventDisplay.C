#include "include/Headers.hxx"
#include "include/Style.hxx"
#include "include/TreeFunctions.hxx"
#include "include/Utilities.hxx"

#define LINE_WIDTH 2  // 4 for presentation purposes, 2 for development

void EvaluateHelix(Double_t params[8], Double_t t, Double_t r[3]);
void GetHelixParamsFromKine(Double_t x[3], Double_t p[3], Short_t charge, Double_t params[8]);

/*** Main ***/

void EventDisplay(TString input_filename = "./AnalysisResults_CustomV0s_000.root",  //
                  Int_t input_event = 0, Int_t input_v0a = 16, Int_t input_v0b = 18) {

  /*** Process Input ***/

  TList *list_of_trees = new TList();
  AddTreesToList(list_of_trees, input_filename + "/Trees/Events");

  Event_tt this_event;

  /*** Geometry ***/

  gSystem->Load("libGeom");

  TEveManager::Create();

  gGeoManager = gEve->GetGeometry("alice.root");

  TGeoNode *node = gGeoManager->GetTopVolume()->FindNode("TPC_M_1");
  node->GetVolume()->SetLineColor(kGray);
  node->GetVolume()->SetTransparency(70.);  // 0..100, larger values make it more transparent

  TEveGeoTopNode *tpc = new TEveGeoTopNode(gGeoManager, node);
  tpc->SetVisOption(2);

  gGeoManager->SetTopVolume(node->GetVolume());
  gGeoManager->CloseGeometry();

  gEve->AddGlobalElement(tpc);

  // beam axis
  TEveLine *beam_axis = new TEveLine();
  beam_axis->SetNextPoint(0.0, 0.0, -650.0);
  beam_axis->SetNextPoint(0.0, 0.0, 650.0);
  beam_axis->SetName("beam axis");
  beam_axis->SetLineStyle(1);
  beam_axis->SetLineWidth(2);
  beam_axis->SetMainAlpha(0.7);
  beam_axis->SetMainColor(kWhite);
  gEve->AddGlobalElement(beam_axis);

  // define new scenes
  TEveScene *scene_mc_particles = gEve->SpawnNewScene("MC Particles");
  gEve->GetDefaultViewer()->AddScene(scene_mc_particles);

  TEveScene *scene_found_v0s = gEve->SpawnNewScene("Found V0s");
  gEve->GetDefaultViewer()->AddScene(scene_found_v0s);

  // (loop) over collected trees
  TListIter *list_of_trees_it = new TListIter(list_of_trees);
  while (TTree *this_tree = (TTree *)list_of_trees_it->Next()) {

    // load branches
    LoadBranches(this_tree, this_event);

    this_tree->GetEntry(input_event);

    // (debug)
    printf("EventDisplay :: Event #%i\n", input_event);

    TEveEventManager *teem_mc_event = new TEveEventManager(Form("MC_Event_%i", input_event));
    gEve->AddEvent(teem_mc_event);

    /*************************/
    /***                   ***/
    /*** MC Gen. Particles ***/
    /***                   ***/
    /*************************/

    Int_t aux_pid;
    Short_t aux_charge;
    Double_t aux_p[3];
    Double_t aux_x[3];
    Double_t aux_helix_params[8];
    Bool_t aux_continue_loop;
    Double_t aux_track_path;
    Double_t aux_pos[3];
    Double_t aux_radius;
    Double_t aux_dist_to_endvertex;

    // (debug)
    printf("EventDisplay :: MC Particles :: Number of MC Particles = %i\n", this_event.N_MCGen);

    // (loop) over MC particles
    for (Int_t evt_mc = 0; evt_mc < this_event.N_MCGen; evt_mc++) {

      // it must be a neutral kaon short or an anti-lambda
      if ((*this_event.MC_PID)[evt_mc] != 310 && (*this_event.MC_PID)[evt_mc] != -3122) {
        continue;
      }

      if ((*this_event.MC_isSignal)[evt_mc]) {
        TEveLine *this_sexa = new TEveLine();
        this_sexa->SetNextPoint(0., 0., 0.);
        this_sexa->SetNextPoint((*this_event.MC_X)[evt_mc], (*this_event.MC_Y)[evt_mc], (*this_event.MC_Z)[evt_mc]);
        this_sexa->SetLineStyle(9);
        this_sexa->SetLineWidth(LINE_WIDTH);
        this_sexa->SetName("mc_sexa");
        this_sexa->SetMainColor(kPink);
        gEve->AddElement(this_sexa);
      } else {
        continue;  // TEMPORARY MEASURE!!
      }

      /* MC V0 */

      // label signal one
      TString this_v0_id = "K0";
      if ((*this_event.MC_PID)[evt_mc] == -3122) this_v0_id = "AL";
      TString this_v0_name = Form("mc_%i_%s", evt_mc, this_v0_id.Data());
      Color_t this_v0_color = (*this_event.MC_isSignal)[evt_mc] ? kGreen : kYellow;
      // Color_t this_v0_color = myOrange;

      TEveLine *this_v0 = new TEveLine();
      this_v0->SetNextPoint((*this_event.MC_X)[evt_mc], (*this_event.MC_Y)[evt_mc], (*this_event.MC_Z)[evt_mc]);
      this_v0->SetNextPoint((*this_event.MC_Xf)[evt_mc], (*this_event.MC_Yf)[evt_mc], (*this_event.MC_Zf)[evt_mc]);
      this_v0->SetLineStyle(1);
      this_v0->SetLineWidth(LINE_WIDTH);
      this_v0->SetName(this_v0_name);
      this_v0->SetMainColor(this_v0_color);

      gEve->AddElement(this_v0, teem_mc_event);

      // plot daughters, only if they're signal
      if (!(*this_event.MC_isSignal)[evt_mc]) {
        continue;
      }

      if ((*this_event.MC_NDaughters)[evt_mc] != 2) {
        continue;
      }

      if ((*this_event.MC_NDaughters)[evt_mc] < 0) {
        continue;
      }

      // (debug)
      printf("EventDisplay :: MC Particles :: V0 :: Index = %i\n", evt_mc);
      printf("EventDisplay :: MC Particles :: V0 :: PID = %i\n", (*this_event.MC_PID)[evt_mc]);

      /* First Daughter */

      // (debug)
      printf("EventDisplay :: MC Particles :: First Daughter :: Index = %i\n", (*this_event.MC_FirstDau)[evt_mc]);

      // get helix params
      aux_pid = (*this_event.MC_PID)[(*this_event.MC_FirstDau)[evt_mc]];

      if (TMath::Abs(aux_pid) == 211 || aux_pid == -2212) {

        printf("EventDisplay :: MC Particles :: First Daughter :: PID = %i\n", aux_pid);
        aux_charge = aux_pid == 211 ? 1 : -1;

        aux_x[0] = (*this_event.MC_X)[(*this_event.MC_FirstDau)[evt_mc]];
        aux_x[1] = (*this_event.MC_Y)[(*this_event.MC_FirstDau)[evt_mc]];
        aux_x[2] = (*this_event.MC_Z)[(*this_event.MC_FirstDau)[evt_mc]];
        printf("EventDisplay :: MC Particles :: First Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);

        aux_p[0] = (*this_event.MC_Px)[(*this_event.MC_FirstDau)[evt_mc]];
        aux_p[1] = (*this_event.MC_Py)[(*this_event.MC_FirstDau)[evt_mc]];
        aux_p[2] = (*this_event.MC_Pz)[(*this_event.MC_FirstDau)[evt_mc]];
        printf("EventDisplay :: MC Particles :: First Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);

        // define line and set first point
        TEveLine *this_first_dau = new TEveLine();
        this_first_dau->SetNextPoint(aux_x[0], aux_x[1], aux_x[2]);

        // (1) search for min. track path
        /*
        Double_t aux_dist_to_v0;
        Double_t min_dist_to_v0 = 99999.;
        Float_t min_track_path;
        for (aux_track_path = -10000.; aux_track_path <= 10000.; aux_track_path += 1.) {
          EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);
          aux_dist_to_v0 = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                       TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                       TMath::Power(aux_x[2] - aux_pos[2], 2));
          if (aux_dist_to_v0 < min_dist_to_v0) {
            min_dist_to_v0 = aux_dist_to_v0;
            min_track_path = aux_track_path;
          }
        }
        */

        // (2) draw helix lines
        // (loop)
        // for (aux_track_path = 50.; aux_track_path < 450; aux_track_path += 1.) {
        for (aux_track_path = 0.; aux_track_path < 450; aux_track_path += 1.) {

          EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

          // (debug)
          printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: Point %.0f\n", aux_track_path);
          printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: >> Coordinates = (%.3f, %.3f, %.3f)\n",  //
                 aux_pos[0], aux_pos[1], aux_pos[2]);

          this_first_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

          aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
          aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                              TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                              TMath::Power(aux_x[2] - aux_pos[2], 2));

          // (debug)
          printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
          printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
          printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

          aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

          if (!aux_continue_loop) {
            // (debug)
            printf("EventDisplay :: MC Particles :: First Daughter :: Drawing :: Loop ended!\n");
            printf("EventDisplay :: MC Particles :: First Daughter\n");
            break;
          }
        }  // end of track drawing

        this_first_dau->SetLineStyle(1);
        this_first_dau->SetLineWidth(LINE_WIDTH);
        this_first_dau->SetName(Form("%i", aux_pid));
        this_first_dau->SetMainColor(aux_charge > 0 ? kCyan : kMagenta);

        gEve->AddElement(this_first_dau, teem_mc_event);

      }  // end of pid condition for first daughter

      /* Last Daughter */

      // (debug)
      printf("EventDisplay :: MC Particles :: Last Daughter :: Index = %i\n", (*this_event.MC_LastDau)[evt_mc]);

      // get helix params
      aux_pid = (*this_event.MC_PID)[(*this_event.MC_LastDau)[evt_mc]];

      // is the particle a pion or an anti-proton?
      if (TMath::Abs(aux_pid) == 211 || aux_pid == -2212) {

        printf("EventDisplay :: MC Particles :: Last Daughter :: PID = %i\n", aux_pid);
        aux_charge = aux_pid == 211 ? 1 : -1;

        aux_x[0] = (*this_event.MC_X)[(*this_event.MC_LastDau)[evt_mc]];
        aux_x[1] = (*this_event.MC_Y)[(*this_event.MC_LastDau)[evt_mc]];
        aux_x[2] = (*this_event.MC_Z)[(*this_event.MC_LastDau)[evt_mc]];
        printf("EventDisplay :: MC Particles :: Last Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);

        aux_p[0] = (*this_event.MC_Px)[(*this_event.MC_LastDau)[evt_mc]];
        aux_p[1] = (*this_event.MC_Py)[(*this_event.MC_LastDau)[evt_mc]];
        aux_p[2] = (*this_event.MC_Pz)[(*this_event.MC_LastDau)[evt_mc]];
        printf("EventDisplay :: MC Particles :: Last Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);

        // define line and set first point
        TEveLine *this_last_dau = new TEveLine();
        this_last_dau->SetNextPoint(aux_x[0], aux_x[1], aux_x[2]);

        // (1) search for min. track path
        /*
        Double_t aux_dist_to_v0;
        Double_t min_dist_to_v0 = 99999.;
        Float_t min_track_path;
        for (aux_track_path = -10000.; aux_track_path <= 10000.; aux_track_path += 1.) {
          EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);
          aux_dist_to_v0 = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                       TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                       TMath::Power(aux_x[2] - aux_pos[2], 2));
          if (aux_dist_to_v0 < min_dist_to_v0) {
            min_dist_to_v0 = aux_dist_to_v0;
            min_track_path = aux_track_path;
          }
        }
        */

        // (2) draw helix lines
        // (loop)
        for (aux_track_path = 0.; aux_track_path < 450; aux_track_path += 1.) {

          EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

          // (debug)
          printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: Point %.0f\n", aux_track_path);
          printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: >> Coordinates = (%.3f, %.3f, %.3f)\n",  //
                 aux_pos[0], aux_pos[1], aux_pos[2]);

          this_last_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

          aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
          aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                              TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                              TMath::Power(aux_x[2] - aux_pos[2], 2));

          // (debug)
          printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
          printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
          printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

          aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

          if (!aux_continue_loop) {
            // (debug)
            printf("EventDisplay :: MC Particles :: Last Daughter :: Drawing :: Loop ended!\n");
            printf("EventDisplay :: MC Particles :: Last Daughter\n");
            break;
          }
        }

        this_last_dau->SetLineStyle(1);
        this_last_dau->SetLineWidth(LINE_WIDTH);
        this_last_dau->SetName(Form("%i", aux_pid));  // PENDING
        this_last_dau->SetMainColor(aux_charge > 0 ? kCyan : kMagenta);

        gEve->AddElement(this_last_dau, teem_mc_event);
      }  // end of pid condition for last daughter

    }    // end of loop over MC gen. particles

    gEve->AddElement(teem_mc_event, scene_mc_particles);

    // (debug) empty line
    printf("EventDisplay ::\n");

    /*************************/
    /***                   ***/
    /***     Found V0s     ***/
    /***                   ***/
    /*************************/

    TEveEventManager *teem_rec_event = new TEveEventManager(Form("Rec_Event_%i", input_event));
    gEve->AddEvent(teem_rec_event);

    // (debug)
    printf("EventDisplay :: Found V0s :: Number of Found V0s = %i\n", this_event.N_V0s);

    // for (Int_t evt_v0 = 0; evt_v0 < this_event.N_V0s; evt_v0++) {
    // for (Int_t evt_v0 = 0; evt_v0 < 2; evt_v0++) {
    for (Int_t evt_v0 : {input_v0a, input_v0b}) {

      // (debug)
      printf("-> v0 %i\n", evt_v0);

      /* Rec. V0 */

      // COMMENT: for the moment, they're just points
      TEvePointSet *this_found_v0 = new TEvePointSet();
      // this_found_v0->SetNextPoint(0, 0, 0);  // PENDING!!
      this_found_v0->SetNextPoint((*this_event.V0_X)[evt_v0], (*this_event.V0_Y)[evt_v0], (*this_event.V0_Z)[evt_v0]);
      this_found_v0->SetMarkerSize(2.);
      this_found_v0->SetMarkerColor(kSpring);
      this_found_v0->SetMainColor(kSpring);
      this_found_v0->SetName(Form("found_V0_%i", evt_v0));

      gEve->AddElement(this_found_v0, teem_rec_event);

      /* Negative Daughter */

      // (debug)
      printf("EventDisplay :: Found V0s :: Negative Daughter :: idx_neg = %i\n", (*this_event.Idx_Neg)[evt_v0]);

      TEveLine *this_neg_dau = new TEveLine();
      this_neg_dau->SetNextPoint((*this_event.V0_X)[evt_v0], (*this_event.V0_Y)[evt_v0], (*this_event.V0_Z)[evt_v0]);

      // get helix params

      // alternative 1
      /*
      aux_helix_params[0] = ;
      aux_helix_params[1] = ;
      aux_helix_params[2] = ;
      aux_helix_params[3] = ;
      aux_helix_params[4] = ;
      aux_helix_params[5] = ;
      */
      // alternative 2
      aux_charge = -1;
      aux_x[0] = (*this_event.V0_X)[evt_v0];
      aux_x[1] = (*this_event.V0_Y)[evt_v0];
      aux_x[2] = (*this_event.V0_Z)[evt_v0];
      printf("EventDisplay :: Found V0s :: Negative Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
      aux_p[0] = (*this_event.Neg_Px)[evt_v0];
      aux_p[1] = (*this_event.Neg_Py)[evt_v0];
      aux_p[2] = (*this_event.Neg_Pz)[evt_v0];
      printf("EventDisplay :: Found V0s :: Negative Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

      GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
      printf("EventDisplay :: Found V0s :: Negative Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
             aux_helix_params[0], aux_helix_params[1],                                                             //
             aux_helix_params[2], aux_helix_params[3],                                                             //
             aux_helix_params[4], aux_helix_params[5]);
      printf("EventDisplay :: Found V0s :: Negative Daughter :: AliExternParams = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",
             (*this_event.Rec_HelixParam0)[(*this_event.Idx_Neg)[evt_v0]], (*this_event.Rec_HelixParam1)[(*this_event.Idx_Neg)[evt_v0]],  //
             (*this_event.Rec_HelixParam2)[(*this_event.Idx_Neg)[evt_v0]], (*this_event.Rec_HelixParam3)[(*this_event.Idx_Neg)[evt_v0]],  //
             (*this_event.Rec_HelixParam4)[(*this_event.Idx_Neg)[evt_v0]], (*this_event.Rec_HelixParam5)[(*this_event.Idx_Neg)[evt_v0]]);

      // (1) search for min. track path
      /*
      Double_t aux_dist_to_v0;
      Double_t min_dist_to_v0 = 99999.;
      Float_t min_track_path;
      printf("EventDisplay :: Found V0s :: Negative Daughter :: Starting minimization...\n");
      for (aux_track_path = -10000.; aux_track_path <= 10000.; aux_track_path += 1.) {
        EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);
        aux_dist_to_v0 = TMath::Sqrt(TMath::Power((*this_event.V0_X)[evt_v0] - aux_pos[0], 2) +  //
                                     TMath::Power((*this_event.V0_Y)[evt_v0] - aux_pos[1], 2) +  //
                                     TMath::Power((*this_event.V0_Z)[evt_v0] - aux_pos[2], 2));
        printf("EventDisplay :: Found V0s :: Negative Daughter :: >> aux_dist_to_v0 = %f\n", aux_dist_to_v0);
        if (aux_dist_to_v0 < min_dist_to_v0) {
          min_dist_to_v0 = aux_dist_to_v0;
          min_track_path = aux_track_path;
        }
      }

      // (debug)
      printf("EventDisplay :: Found V0s :: Negative Daughter :: min_dist_to_v0 = %f\n", min_dist_to_v0);
      printf("EventDisplay :: Found V0s :: Negative Daughter :: min_track_path = %f\n", min_track_path);
      */

      // (2) draw track
      for (aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

        // (debug)
        printf("EventDisplay :: Found V0s :: Negative Daughter :: aux_track_path = %f\n", aux_track_path);

        EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

        this_neg_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

        aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
        aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                            TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                            TMath::Power(aux_x[2] - aux_pos[2], 2));

        // (debug)
        printf("EventDisplay :: Found V0s :: Negative Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
        printf("EventDisplay :: Found V0s :: Negative Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
        printf("EventDisplay :: Found V0s :: Negative Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

        // aux_continue_loop = aux_track_path < 1500 && aux_dist_to_endvertex >= 10 && aux_radius <= 460. && TMath::Abs(aux_pos[2]) <=
        // 360.;
        // aux_continue_loop = aux_dist_to_endvertex >= 10 && aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;
        aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

        if (!aux_continue_loop) {
          // (debug)
          printf("EventDisplay :: Found V0s :: Negative Daughter :: Drawing :: Loop ended!\n");
          printf("EventDisplay :: Found V0s :: Negative Daughter\n");
          break;
        }
      }  // end of track drawing

      this_neg_dau->SetLineStyle(1);
      this_neg_dau->SetLineWidth(LINE_WIDTH);
      this_neg_dau->SetName(Form("found_V0_%i_neg", evt_v0));
      this_neg_dau->SetMainColor(kMagenta);

      gEve->AddElement(this_neg_dau, teem_rec_event);

      /* Positive Daughter */

      // (debug)
      printf("EventDisplay :: Found V0s :: Positive Daughter :: idx_pos = %i\n", (*this_event.Idx_Pos)[evt_v0]);

      TEveLine *this_pos_dau = new TEveLine();
      this_pos_dau->SetNextPoint((*this_event.V0_X)[evt_v0], (*this_event.V0_Y)[evt_v0], (*this_event.V0_Z)[evt_v0]);

      // get helix params
      // alternative 1
      /*
      aux_helix_params[0] = (*this_event.Rec_HelixParam0)[(*this_event.Idx_Pos)[evt_v0]];
      aux_helix_params[1] = (*this_event.Rec_HelixParam1)[(*this_event.Idx_Pos)[evt_v0]];
      aux_helix_params[2] = (*this_event.Rec_HelixParam2)[(*this_event.Idx_Pos)[evt_v0]];
      aux_helix_params[3] = (*this_event.Rec_HelixParam3)[(*this_event.Idx_Pos)[evt_v0]];
      aux_helix_params[4] = (*this_event.Rec_HelixParam4)[(*this_event.Idx_Pos)[evt_v0]];
      aux_helix_params[5] = (*this_event.Rec_HelixParam5)[(*this_event.Idx_Pos)[evt_v0]];
      */

      // alternative 2
      aux_charge = +1;
      aux_x[0] = (*this_event.V0_X)[evt_v0];
      aux_x[1] = (*this_event.V0_Y)[evt_v0];
      aux_x[2] = (*this_event.V0_Z)[evt_v0];
      printf("EventDisplay :: Found V0s :: Positive Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
      aux_p[0] = (*this_event.Pos_Px)[evt_v0];
      aux_p[1] = (*this_event.Pos_Py)[evt_v0];
      aux_p[2] = (*this_event.Pos_Pz)[evt_v0];
      printf("EventDisplay :: Found V0s :: Positive Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

      GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
      printf("EventDisplay :: Found V0s :: Positive Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
             aux_helix_params[0], aux_helix_params[1],                                                             //
             aux_helix_params[2], aux_helix_params[3],                                                             //
             aux_helix_params[4], aux_helix_params[5]);
      printf("EventDisplay :: Found V0s :: Positive Daughter :: AliExternParams = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",
             (*this_event.Rec_HelixParam0)[(*this_event.Idx_Pos)[evt_v0]], (*this_event.Rec_HelixParam1)[(*this_event.Idx_Pos)[evt_v0]],  //
             (*this_event.Rec_HelixParam2)[(*this_event.Idx_Pos)[evt_v0]], (*this_event.Rec_HelixParam3)[(*this_event.Idx_Pos)[evt_v0]],  //
             (*this_event.Rec_HelixParam4)[(*this_event.Idx_Pos)[evt_v0]], (*this_event.Rec_HelixParam5)[(*this_event.Idx_Pos)[evt_v0]]);

      // (1) search for min. track path
      /*
      min_dist_to_v0 = 99999.;
      printf("EventDisplay :: Found V0s :: Positive Daughter :: Starting minimization...\n");
      for (aux_track_path = -10000.; aux_track_path <= 10000.; aux_track_path += 1.) {
        EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);
        aux_dist_to_v0 = TMath::Sqrt(TMath::Power((*this_event.V0_X)[evt_v0] - aux_pos[0], 2) +  //
                                     TMath::Power((*this_event.V0_Y)[evt_v0] - aux_pos[1], 2) +  //
                                     TMath::Power((*this_event.V0_Z)[evt_v0] - aux_pos[2], 2));
        printf("EventDisplay :: Found V0s :: Positive Daughter :: >> aux_dist_to_v0 = %f\n", aux_dist_to_v0);
        if (aux_dist_to_v0 < min_dist_to_v0) {
          min_dist_to_v0 = aux_dist_to_v0;
          min_track_path = aux_track_path;
        }
      }

      // (debug)
      printf("EventDisplay :: Found V0s :: Positive Daughter :: min_dist_to_v0 = %f\n", min_dist_to_v0);
      printf("EventDisplay :: Found V0s :: Positive Daughter :: min_track_path = %f\n", min_track_path);
      */

      // (2) draw track
      for (Float_t aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

        // (debug)
        printf("EventDisplay :: Found V0s :: Positive Daughter :: aux_track_path = %f\n", aux_track_path);

        EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

        this_pos_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

        aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
        aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                            TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                            TMath::Power(aux_x[2] - aux_pos[2], 2));

        // (debug)
        printf("EventDisplay :: Found V0s :: Positive Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
        printf("EventDisplay :: Found V0s :: Positive Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
        printf("EventDisplay :: Found V0s :: Positive Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

        aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

        if (!aux_continue_loop) {
          // (debug)
          printf("EventDisplay :: Found V0s :: Positive Daughter :: Drawing :: Loop ended!\n");
          printf("EventDisplay :: Found V0s :: Positive Daughter\n");
          break;
        }

      }  // end of track drawing

      this_pos_dau->SetLineStyle(1);
      this_pos_dau->SetLineWidth(LINE_WIDTH);
      this_pos_dau->SetName(Form("found_V0_%i_pos", evt_v0));
      this_pos_dau->SetMainColor(kCyan);

      gEve->AddElement(this_pos_dau, teem_rec_event);

    }  // end of loop over found V0s

    gEve->AddElement(teem_rec_event, scene_found_v0s);

  }  // end of loop over collected trees
}

/*** Functions ***/

void EvaluateHelix(Double_t params[8], Double_t t, Double_t r[3]) {
  //
  // Calculate position of a point on a track and some derivatives at given phase
  //
  Double_t phase = params[4] * t + params[2];
  Double_t sn = TMath::Sin(phase);
  Double_t cs = TMath::Cos(phase);

  r[0] = params[5] + sn / params[4];
  r[1] = params[0] - cs / params[4];
  r[2] = params[1] + params[3] * t;
}

void GetHelixParamsFromKine(Double_t x[3], Double_t p[3], Short_t charge, Double_t params[8]) {
  //
  // Returns helix parameters from kinematic input, Lorentz vector and vertex + charge
  // Calculation of Helix parameters taken from http://alidoc.cern.ch/AliRoot/v5-09-36/_ali_helix_8cxx_source.html
  //

  // PENDING: is this a constant??
  // magnetic field
  Double_t b_field = 0.5;
  Double_t b_fak = b_field * 3. / 1000.;

  Double_t pt = TMath::Sqrt(p[0] * p[0] + p[1] * p[1]);
  Double_t curvature = ((charge / pt) * b_fak);

  params[4] = curvature;  // C
  params[3] = p[2] / pt;  // tgl

  Double_t rc = 1 / params[4];
  Double_t xc = x[0] - rc * p[1] / pt;
  Double_t yc = x[1] + rc * p[0] / pt;
  params[5] = x[0];  // x0
  params[0] = x[1];  // y0
  params[1] = x[2];  // z0
  params[5] = xc;
  params[0] = yc;
  params[6] = pt;
  params[7] = p[2];

  if (TMath::Abs(p[1]) < TMath::Abs(p[0])) {
    params[2] = TMath::ASin(p[1] / pt);
    if (charge * yc < charge * x[1]) {
      params[2] = TMath::Pi() - params[2];
    }
  } else {
    params[2] = TMath::ACos(p[0] / pt);
    if (charge * xc > charge * x[0]) {
      params[2] = -params[2];
    }
  }
}
