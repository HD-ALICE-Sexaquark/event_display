#include "include/Headers.hxx"
#include "include/Style.hxx"
#include "include/Utilities.hxx"

#define LINE_WIDTH 2  // 4 for presentation purposes, 2 for development

void EvaluateHelix(Double_t params[8], Double_t t, Double_t r[3]);
void GetHelixParamsFromKine(Double_t x[3], Double_t p[3], Short_t charge, Double_t params[8]);

/*** Main ***/

void SexaquarkDisplay(TString input_filename = "./SexaquarkResults_CustomV0s_246178_000.root",  //
                      Int_t input_event = 212, Int_t input_v0a = 16, Int_t input_v0b = 18) {

    /*** Process Input ***/

    TChain *input_chain = new TChain("");
    input_chain->Add(input_filename + "/Sexaquarks");

    // define input sexaquark variables
    Int_t RunNumber;
    Int_t DirNumber;
    Int_t Event;

    Int_t Idx_V0A;
    Int_t Idx_V0B;
    Float_t Sexa_Px;
    Float_t Sexa_Py;
    Float_t Sexa_Pz;
    Float_t Sexa_X;
    Float_t Sexa_Y;
    Float_t Sexa_Z;
    Bool_t Sexa_isSignal;

    Float_t V0A_X;
    Float_t V0A_Y;
    Float_t V0A_Z;
    Float_t V0A_Pos_Px;
    Float_t V0A_Pos_Py;
    Float_t V0A_Pos_Pz;
    Float_t V0A_Neg_Px;
    Float_t V0A_Neg_Py;
    Float_t V0A_Neg_Pz;

    Float_t V0B_X;
    Float_t V0B_Y;
    Float_t V0B_Z;
    Float_t V0B_Pos_Px;
    Float_t V0B_Pos_Py;
    Float_t V0B_Pos_Pz;
    Float_t V0B_Neg_Px;
    Float_t V0B_Neg_Py;
    Float_t V0B_Neg_Pz;

    input_chain->SetBranchAddress("RunNumber", &RunNumber);
    input_chain->SetBranchAddress("DirNumber", &DirNumber);
    input_chain->SetBranchAddress("Event", &Event);

    input_chain->SetBranchAddress("Idx_V0A", &Idx_V0A);
    input_chain->SetBranchAddress("Idx_V0B", &Idx_V0B);
    input_chain->SetBranchAddress("Px", &Sexa_Px);
    input_chain->SetBranchAddress("Py", &Sexa_Py);
    input_chain->SetBranchAddress("Pz", &Sexa_Pz);
    input_chain->SetBranchAddress("X", &Sexa_X);
    input_chain->SetBranchAddress("Y", &Sexa_Y);
    input_chain->SetBranchAddress("Z", &Sexa_Z);
    input_chain->SetBranchAddress("isSignal", &Sexa_isSignal);

    input_chain->SetBranchAddress("V0A_X", &V0A_X);
    input_chain->SetBranchAddress("V0A_Y", &V0A_Y);
    input_chain->SetBranchAddress("V0A_Z", &V0A_Z);
    input_chain->SetBranchAddress("V0A_Pos_Px", &V0A_Pos_Px);
    input_chain->SetBranchAddress("V0A_Pos_Py", &V0A_Pos_Py);
    input_chain->SetBranchAddress("V0A_Pos_Pz", &V0A_Pos_Pz);
    input_chain->SetBranchAddress("V0A_Neg_Px", &V0A_Neg_Px);
    input_chain->SetBranchAddress("V0A_Neg_Py", &V0A_Neg_Py);
    input_chain->SetBranchAddress("V0A_Neg_Pz", &V0A_Neg_Pz);

    input_chain->SetBranchAddress("V0B_X", &V0B_X);
    input_chain->SetBranchAddress("V0B_Y", &V0B_Y);
    input_chain->SetBranchAddress("V0B_Z", &V0B_Z);
    input_chain->SetBranchAddress("V0B_Pos_Px", &V0B_Pos_Px);
    input_chain->SetBranchAddress("V0B_Pos_Py", &V0B_Pos_Py);
    input_chain->SetBranchAddress("V0B_Pos_Pz", &V0B_Pos_Pz);
    input_chain->SetBranchAddress("V0B_Neg_Px", &V0B_Neg_Px);
    input_chain->SetBranchAddress("V0B_Neg_Py", &V0B_Neg_Py);
    input_chain->SetBranchAddress("V0B_Neg_Pz", &V0B_Neg_Pz);

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
    TEveScene *scene_sexa_candidates = gEve->SpawnNewScene("Sexaquark Candidates");
    gEve->GetDefaultViewer()->AddScene(scene_sexa_candidates);

    // (debug)
    printf("SexaquarkDisplay :: Number of Candidates = %i\n", (Int_t)input_chain->GetEntries());

    // (loop) over sexaquark candidates
    for (Int_t candidate = 0; candidate < input_chain->GetEntries(); candidate++) {
        // for (Int_t candidate = 0; candidate < 1; candidate++) {

        input_chain->GetEntry(candidate);

        // (cut)
        /*
        if (!Sexa_isSignal) {
          continue;
        }
        */

        if (Event != input_event) {
            printf("SexaquarkDisplay :: Candidate discarded, doesn't belong to input event\n");
            continue;
        }

        if ((Idx_V0A == input_v0a && Idx_V0B == input_v0b) || (Idx_V0A == input_v0b && Idx_V0B == input_v0a)) {
            // do nothing
        } else {
            printf("SexaquarkDisplay :: Candidate discarded, V0s don't correspond to input indices\n");
            continue;
        }

        // (debug)
        printf("SexaquarkDisplay :: Candidate #%i\n", candidate);
        printf("SexaquarkDisplay :: RN, Dir Number, Event = %i, %i, %i\n", RunNumber, DirNumber, Event);

        TEveEventManager *manager_candidate = new TEveEventManager(Form("Candidate_%i", candidate));
        gEve->AddEvent(manager_candidate);

        // auxiliary variables
        Short_t aux_charge;
        Double_t aux_p[3];
        Double_t aux_x[3];
        Double_t aux_helix_params[8];
        Bool_t aux_continue_loop;
        Double_t aux_track_path;
        Double_t aux_pos[3];
        Double_t aux_radius;
        Double_t aux_dist_to_endvertex;

        /* Sexaquark */

        TEveLine *line_sexa = new TEveLine();
        line_sexa->SetNextPoint(0., 0., 0.);  // PENDING!
        line_sexa->SetNextPoint(Sexa_X, Sexa_Y, Sexa_Z);
        line_sexa->SetLineStyle(9);
        line_sexa->SetLineWidth(LINE_WIDTH);
        line_sexa->SetMainColor(kPink);
        line_sexa->SetName(Form("Sexa%i", candidate));

        gEve->AddElement(line_sexa, manager_candidate);

        /* V0A */

        TEveLine *line_v0a = new TEveLine();
        line_v0a->SetNextPoint(Sexa_X, Sexa_Y, Sexa_Z);
        line_v0a->SetNextPoint(V0A_X, V0A_Y, V0A_Z);
        line_v0a->SetLineStyle(1);
        line_v0a->SetLineWidth(LINE_WIDTH);
        line_v0a->SetMainColor(kGreen);
        line_v0a->SetName(Form("Sexa%i_V0A%i", candidate, Idx_V0A));

        gEve->AddElement(line_v0a, manager_candidate);

        /* V0A - Negative Daughter */

        TEveLine *line_v0a_neg_dau = new TEveLine();

        // get helix params
        aux_charge = -1;
        aux_x[0] = V0A_X;
        aux_x[1] = V0A_Y;
        aux_x[2] = V0A_Z;
        printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
        aux_p[0] = V0A_Neg_Px;
        aux_p[1] = V0A_Neg_Py;
        aux_p[2] = V0A_Neg_Pz;
        printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
        printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
               aux_helix_params[0], aux_helix_params[1],                                                           //
               aux_helix_params[2], aux_helix_params[3],                                                           //
               aux_helix_params[4], aux_helix_params[5]);

        // (2) draw track
        for (aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

            // (debug)
            printf("SexaquarkDisplay :: V0A :: Negative Daughter :: aux_track_path = %f\n", aux_track_path);

            EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

            line_v0a_neg_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

            aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
            aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                                TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                                TMath::Power(aux_x[2] - aux_pos[2], 2));

            // (debug)
            printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
            printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
            printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

            aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

            if (!aux_continue_loop) {
                // (debug)
                printf("SexaquarkDisplay :: V0A :: Negative Daughter :: Drawing :: Loop ended!\n");
                printf("SexaquarkDisplay :: V0A :: Negative Daughter\n");
                break;
            }
        }  // end of track drawing

        line_v0a_neg_dau->SetLineStyle(1);
        line_v0a_neg_dau->SetLineWidth(LINE_WIDTH);
        line_v0a_neg_dau->SetName(Form("Sexa%i_V0A%i_Neg", candidate, Idx_V0A));
        line_v0a_neg_dau->SetMainColor(kMagenta);

        gEve->AddElement(line_v0a_neg_dau, manager_candidate);

        /* V0A - Positive Daughter */

        TEveLine *line_v0a_pos_dau = new TEveLine();

        // get helix params
        aux_charge = +1;
        aux_x[0] = V0A_X;
        aux_x[1] = V0A_Y;
        aux_x[2] = V0A_Z;
        printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
        aux_p[0] = V0A_Neg_Px;
        aux_p[1] = V0A_Neg_Py;
        aux_p[2] = V0A_Neg_Pz;
        printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
        printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
               aux_helix_params[0], aux_helix_params[1],                                                           //
               aux_helix_params[2], aux_helix_params[3],                                                           //
               aux_helix_params[4], aux_helix_params[5]);

        // (2) draw track
        for (aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

            // (debug)
            printf("SexaquarkDisplay :: V0A :: Positive Daughter :: aux_track_path = %f\n", aux_track_path);

            EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

            line_v0a_pos_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

            aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
            aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                                TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                                TMath::Power(aux_x[2] - aux_pos[2], 2));

            // (debug)
            printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
            printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
            printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

            aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

            if (!aux_continue_loop) {
                // (debug)
                printf("SexaquarkDisplay :: V0A :: Positive Daughter :: Drawing :: Loop ended!\n");
                printf("SexaquarkDisplay :: V0A :: Positive Daughter\n");
                break;
            }
        }  // end of track drawing

        line_v0a_pos_dau->SetLineStyle(1);
        line_v0a_pos_dau->SetLineWidth(LINE_WIDTH);
        line_v0a_pos_dau->SetName(Form("Sexa%i_V0A%i_Pos", candidate, Idx_V0A));
        line_v0a_pos_dau->SetMainColor(kCyan);

        gEve->AddElement(line_v0a_pos_dau, manager_candidate);

        /* V0B */

        TEveLine *line_v0b = new TEveLine();
        line_v0b->SetNextPoint(Sexa_X, Sexa_Y, Sexa_Z);
        line_v0b->SetNextPoint(V0B_X, V0B_Y, V0B_Z);
        line_v0b->SetLineStyle(1);
        line_v0b->SetLineWidth(LINE_WIDTH);
        line_v0b->SetMainColor(kGreen);
        line_v0b->SetName(Form("Sexa%i_V0B%i", candidate, Idx_V0B));

        gEve->AddElement(line_v0b, manager_candidate);

        /* V0B - Negative Daughter */

        TEveLine *line_v0b_neg_dau = new TEveLine();

        // get helix params
        aux_charge = -1;
        aux_x[0] = V0B_X;
        aux_x[1] = V0B_Y;
        aux_x[2] = V0B_Z;
        printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
        aux_p[0] = V0B_Neg_Px;
        aux_p[1] = V0B_Neg_Py;
        aux_p[2] = V0B_Neg_Pz;
        printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
        printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
               aux_helix_params[0], aux_helix_params[1],                                                           //
               aux_helix_params[2], aux_helix_params[3],                                                           //
               aux_helix_params[4], aux_helix_params[5]);

        // (2) draw track
        for (aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

            // (debug)
            printf("SexaquarkDisplay :: V0B :: Negative Daughter :: aux_track_path = %f\n", aux_track_path);

            EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

            line_v0b_neg_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

            aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
            aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                                TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                                TMath::Power(aux_x[2] - aux_pos[2], 2));

            // (debug)
            printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
            printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
            printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

            aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

            if (!aux_continue_loop) {
                // (debug)
                printf("SexaquarkDisplay :: V0B :: Negative Daughter :: Drawing :: Loop ended!\n");
                printf("SexaquarkDisplay :: V0B :: Negative Daughter\n");
                break;
            }
        }  // end of track drawing

        line_v0b_neg_dau->SetLineStyle(1);
        line_v0b_neg_dau->SetLineWidth(LINE_WIDTH);
        line_v0b_neg_dau->SetName(Form("Sexa%i_V0B%i_Neg", candidate, Idx_V0B));
        line_v0b_neg_dau->SetMainColor(kMagenta);

        gEve->AddElement(line_v0b_neg_dau, manager_candidate);

        /* V0B - Positive Daughter */

        TEveLine *line_v0b_pos_dau = new TEveLine();

        // get helix params
        aux_charge = +1;
        aux_x[0] = V0B_X;
        aux_x[1] = V0B_Y;
        aux_x[2] = V0B_Z;
        printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Origin = (%.3f, %.3f, %.3f)\n", aux_x[0], aux_x[1], aux_x[2]);
        aux_p[0] = V0B_Neg_Px;
        aux_p[1] = V0B_Neg_Py;
        aux_p[2] = V0B_Neg_Pz;
        printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Momentum = (%.3f, %.3f, %.3f)\n", aux_p[0], aux_p[1], aux_p[2]);

        GetHelixParamsFromKine(aux_x, aux_p, aux_charge, aux_helix_params);
        printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Params = {%.3f, %.3f, %.3f, %.3f, %.3f, %.3f}\n",  //
               aux_helix_params[0], aux_helix_params[1],                                                           //
               aux_helix_params[2], aux_helix_params[3],                                                           //
               aux_helix_params[4], aux_helix_params[5]);

        // (2) draw track
        for (aux_track_path = 0.; aux_track_path <= 750.; aux_track_path += 1.) {

            // (debug)
            printf("SexaquarkDisplay :: V0B :: Positive Daughter :: aux_track_path = %f\n", aux_track_path);

            EvaluateHelix(aux_helix_params, aux_track_path, aux_pos);

            line_v0b_pos_dau->SetNextPoint(aux_pos[0], aux_pos[1], aux_pos[2]);

            aux_radius = TMath::Sqrt(TMath::Power(aux_pos[0], 2) + TMath::Power(aux_pos[1], 2));
            aux_dist_to_endvertex = TMath::Sqrt(TMath::Power(aux_x[0] - aux_pos[0], 2) +  //
                                                TMath::Power(aux_x[1] - aux_pos[1], 2) +  //
                                                TMath::Power(aux_x[2] - aux_pos[2], 2));

            // (debug)
            printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Drawing :: >> Distance to V0  = %.3f\n", aux_dist_to_endvertex);
            printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Drawing :: >> Distance to PV  = %.3f\n", aux_radius);
            printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Drawing :: >> z = %.3f\n", aux_pos[2]);

            aux_continue_loop = aux_radius <= 460. && TMath::Abs(aux_pos[2]) <= 360.;

            if (!aux_continue_loop) {
                // (debug)
                printf("SexaquarkDisplay :: V0B :: Positive Daughter :: Drawing :: Loop ended!\n");
                printf("SexaquarkDisplay :: V0B :: Positive Daughter\n");
                break;
            }
        }  // end of track drawing

        line_v0b_pos_dau->SetLineStyle(1);
        line_v0b_pos_dau->SetLineWidth(LINE_WIDTH);
        line_v0b_pos_dau->SetName(Form("Sexa%i_V0B%i_Pos", candidate, Idx_V0B));
        line_v0b_pos_dau->SetMainColor(kCyan);

        gEve->AddElement(line_v0b_pos_dau, manager_candidate);

        // finally, add manager to the scene
        gEve->AddElement(manager_candidate, scene_sexa_candidates);
    }  // end of loop over sexaquark candidates
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
