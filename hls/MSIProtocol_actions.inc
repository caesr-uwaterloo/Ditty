if ((event == Inv) && (state == IS_D)) {
  ctrl.update_tag_update = true;
} else if ((event == Load) && (state == I)) {
  ctrl.mshr_insert = true;
  ctrl.pr_insert = true;
  ctrl.to_noc_send = true;
  ctrl.update_tag_update = true;
} else if ((event == Store) && (state == I)) {
  ctrl.mshr_insert = true;
  ctrl.pr_insert = true;
  ctrl.to_noc_send = true;
  ctrl.update_tag_update = true;
}